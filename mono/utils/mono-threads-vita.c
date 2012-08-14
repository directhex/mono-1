/*
 * mono-threads-psp2.c: Low-level threading, PSP2 version
 *
 * (C) 2012 SCEA, LLC
 */

#include <config.h>

#include <mono/mini/mini.h>
#include <mono/utils/mono-compiler.h>
#include <mono/utils/mono-semaphore.h>
#include <mono/utils/mono-threads.h>
#include <mono/utils/mono-tls.h>
#include <mono/metadata/threads-types.h>

#include <errno.h>

#if defined(TARGET_VITA)
#include "bridge.h"
#include <mono/utils/mono-sigcontext.h>

typedef struct {
	void *(*start_routine)(void*);
	void *arg;
	int flags;
	MonoSemType registered;
} ThreadStartInfo;

static void*
inner_start_thread (void *arg)
{
	ThreadStartInfo *start_info = arg;
	void *t_arg = start_info->arg;
	int post_result;
	void *(*start_func)(void*) = start_info->start_routine;
	void *result;

	mono_thread_info_attach (&result);

	post_result = MONO_SEM_POST (&(start_info->registered));
	g_assert (!post_result);

	result = start_func (t_arg);
	g_assert (!mono_domain_get ());


	return result;
}

int
mono_threads_pthread_create (pthread_t *new_thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
	ThreadStartInfo *start_info;
	int result;

	start_info = g_malloc0 (sizeof (ThreadStartInfo));
	if (!start_info)
		return ENOMEM;
	MONO_SEM_INIT (&(start_info->registered), 0);
	start_info->arg = arg;
	start_info->start_routine = start_routine;

	result = mono_threads_get_callbacks ()->mono_gc_pthread_create (new_thread, attr, inner_start_thread, start_info);
	if (result == 0) {
		while (MONO_SEM_WAIT (&(start_info->registered)) != 0) {
			/*if (EINTR != errno) ABORT("sem_wait failed"); */
		}
	}
	MONO_SEM_DESTROY (&(start_info->registered));
	g_free (start_info);
	return result;
}

void
mono_threads_init_platform (void)
{
	/* FIXME: This should be generalized and not go directly into mini */
	pss_threads_initialize (mono_arch_handle_exception_on_thread);
}

void
mono_threads_core_interrupt (MonoThreadInfo *info)
{
}

static void
suspend_thread (int id)
{
	int32_t result = 0;

	do {
		result = pss_suspend_thread (id, NULL);
	} while (result == PSS_ERROR_THREAD_SUSPENDED);
	g_assert (result >= 0);

	//printf ("suspended(%p).\n", id);
}

static void
suspend_thread_and_get_context (int id, PSSIntegerRegisters *iregs, PSSFloatRegisters *fregs)
{
	int32_t result;

	suspend_thread (id);

	iregs->size = sizeof (PSSIntegerRegisters);
	fregs->size = sizeof (PSSFloatRegisters);

	result = pss_get_thread_context (id, iregs, fregs);
	if (result < 0) {
		printf ("pss_get_thread_context (%p) failed: %x\n", id, result);
		g_assert (result >= 0);
	}
}

static void
resume_thread (int id)
{
	int32_t result;

	result = pss_resume_thread (id);
	if (result != PSS_OK) {
		printf ("pss_resume_thread (%p) failed: 0x%x\n", id, result); 
		g_assert (result == PSS_OK);
	}

	//printf ("resumed(%p).\n", id);
}

static void
make_sigctx_from_regs (MonoThreadInfo *info, arm_ucontext *ctx,
	PSSIntegerRegisters *iregs, PSSFloatRegisters *fregs)
{
	memcpy (&ctx->iregs, iregs, sizeof (PSSIntegerRegisters));
	memcpy (&ctx->fregs, fregs, sizeof (PSSFloatRegisters));

	/* Save it since MonoContext doesn't contain all the fields */
	memcpy (&info->sigctx, ctx, sizeof (arm_ucontext));
}

static void
wait_for_wake (volatile gint32 *wake, gint32 value)
{
	while (*wake != value)
		pss_delay_thread (10);
}

gboolean
mono_threads_core_suspend (MonoThreadInfo *info)
{
	pthread_t tid = mono_thread_info_get_tid (info);
	PSSIntegerRegisters iregs;
	PSSFloatRegisters fregs;
	arm_ucontext ctx;
	gboolean ret;
	MonoThreadUnwindState *tctx = &info->suspend_state;

	// FIXME: use the GC lock to prevent races with the GC suspend code.

	//printf ("SUSPEND: %p\n", tid->id);

	suspend_thread_and_get_context (tid->id, &iregs, &fregs);

	/*
	 * MonoContext doesn't contain all data from ctx, so compute and save it into,
	 * info->sigctx, so it can be used during the resume.
	 */
	make_sigctx_from_regs (info, &info->sigctx, &iregs, &fregs);

	ret = mono_threads_get_runtime_callbacks ()->thread_state_init_from_handle (&info->suspend_state, tid, info->native_handle);
	if (!ret)
		return FALSE;

	//printf ("SUSPEND DONE.\n");
	return TRUE;
}

gboolean
mono_threads_core_resume (MonoThreadInfo *info)
{
	pthread_t tid = mono_thread_info_get_tid (info);
	int32_t result;
	arm_ucontext ctx;

	//printf ("RESUME!\n");

	if (info->async_target) {
		MonoContext tmp = info->suspend_state.ctx;
		mono_threads_get_runtime_callbacks ()->setup_async_callback (&tmp, info->async_target, info->user_data);
		info->async_target = info->user_data = NULL;

		memcpy (&ctx, &info->sigctx, sizeof (ctx));
		
		mono_monoctx_to_sigctx (&tmp, &ctx);

		result = pss_set_thread_context (tid->id, &ctx.iregs, &ctx.fregs);
		g_assert (result >= 0);
	}

	//g_print ("resuming thread %x\n", tid->id);
	resume_thread (tid->id);
	return TRUE;
}

void
mono_threads_platform_register (MonoThreadInfo *info)
{
}

void
mono_threads_platform_free (MonoThreadInfo *info)
{
}

MonoNativeThreadId
mono_native_thread_id_get (void)
{
	return pthread_self ();
}

gboolean
mono_native_thread_id_equals (MonoNativeThreadId id1, MonoNativeThreadId id2)
{
	return pthread_equal (id1, id2);
}

gboolean
mono_native_thread_create (MonoNativeThreadId *tid, gpointer func, gpointer arg)
{
	return pthread_create (tid, NULL, func, arg) == 0;
}

#endif
