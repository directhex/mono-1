/*
 * mini-vita.c: Vita signal handling support for Mono.
 *
 * Copyright 2012 SCEA, LLC
 *
 * See LICENSE for licensing information.
 */
#include <config.h>
#include <signal.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <math.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif

#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tokentype.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/io-layer/io-layer.h>
#include "mono/metadata/profiler.h"
#include <mono/metadata/profiler-private.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/gc-internal.h>
#include <mono/metadata/threads-types.h>
#include <mono/metadata/verify.h>
#include <mono/metadata/verify-internals.h>
#include <mono/metadata/mempool-internals.h>
#include <mono/metadata/attach.h>
#include <mono/utils/mono-math.h>
#include <mono/utils/mono-compiler.h>
#include <mono/utils/mono-counters.h>
#include <mono/utils/mono-logger-internal.h>
#include <mono/utils/mono-mmap.h>
#include <mono/utils/dtrace.h>

#include <mono/mini/mini.h>
#include <string.h>
#include <ctype.h>
#include <mono/mini/trace.h>
#include <mono/mini/debugger-agent.h>

#include <mono/mini/jit-icalls.h>

void
mono_runtime_setup_stat_profiler (void)
{
}

void
mono_runtime_shutdown_stat_profiler (void)
{
}


gboolean
SIG_HANDLER_SIGNATURE (mono_chain_signal)
{
	return FALSE;
}

void
mono_runtime_install_handlers (void)
{
}

void
mono_runtime_shutdown_handlers (void)
{
}

void
mono_runtime_cleanup_handlers (void)
{
}

gboolean
mono_thread_state_init_from_handle (MonoThreadUnwindState *tctx, MonoNativeThreadId thread_id, MonoNativeThreadHandle thread_handle)
{
	pthread_t tid = (pthread_t)thread_id;
	int result;
	arm_ucontext ctx;
	PSSIntegerRegisters iregs;
	PSSFloatRegisters fregs;
	MonoJitTlsData *jit_tls;
	MonoDomain *domain;
	MonoNativeTlsKey domain_key, jit_key;

	iregs.size = sizeof (PSSIntegerRegisters);
	fregs.size = sizeof (PSSFloatRegisters);

	result = pss_get_thread_context (tid->id, &iregs, &fregs);
	if (result < 0) {
		g_assert (result >= 0);
	}

	memcpy (&ctx.iregs, &iregs, sizeof (iregs));
	memcpy (&ctx.fregs, &fregs, sizeof (fregs));

	mono_sigctx_to_monoctx (&ctx, &tctx->ctx);

	domain_key = mono_domain_get_tls_key ();
	jit_key = mono_get_jit_tls_key ();

	domain = pthread_getspecific_for_thread (tid, domain_key);
	g_assert (domain);
	jit_tls = pthread_getspecific_for_thread (tid, jit_key);

	/*Thread already started to cleanup, can no longer capture unwind state*/
	if (!jit_tls)
		return FALSE;

	tctx->unwind_data [MONO_UNWIND_DATA_DOMAIN] = domain;
	tctx->unwind_data [MONO_UNWIND_DATA_LMF] = jit_tls ? jit_tls->lmf : NULL;
	tctx->unwind_data [MONO_UNWIND_DATA_JIT_TLS] = jit_tls;
	tctx->valid = TRUE;

	return TRUE;
}