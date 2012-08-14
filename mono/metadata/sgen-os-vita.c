#include "config.h"
#include "metadata/sgen-gc.h"
#include "metadata/gc-internal.h"

#if defined(HAVE_SGEN_GC) && defined(TARGET_VITA)
#include "bridge.h"

gboolean
mono_sgen_resume_thread (SgenThreadInfo *info)
{
	pthread_t tid;
	int res;

	tid = mono_thread_info_get_tid (info);
	res = pss_resume_thread (tid->id);
	
	g_assert (res == PSS_OK);

	return res == PSS_OK;
}

gboolean
mono_sgen_suspend_thread (SgenThreadInfo *info)
{
	PSSIntegerRegisters iregs;
	PSSFloatRegisters fregs;
	int i;
	int result;
	pthread_t tid;
	
	tid = mono_thread_info_get_tid (info);

	do {
		result = pss_suspend_thread (tid->id, NULL);
	} while (result == PSS_ERROR_THREAD_SUSPENDED || info->thread_is_dying);
	if (!(result == PSS_OK || info->thread_is_dying)) {
		g_assert (result == PSS_OK || info->thread_is_dying);
	}
	if (result != PSS_OK)
		return FALSE;

	iregs.size = sizeof (PSSIntegerRegisters);
	fregs.size = sizeof (PSSFloatRegisters);

	result = pss_get_thread_context (tid->id, &iregs, &fregs);
	g_assert (result >= 0);

	info->stopped_domain = NULL; /* FIXME: implement! */
	info->stopped_ip = (gpointer) iregs.reg [15];
	info->stack_start = (char*) iregs.reg [13];

	for (i = 0; i < ARCH_NUM_REGS; ++i)
		info->regs [i] = (gpointer) (iregs.reg [i == 13 ? 14 : i]);
	info->stopped_regs = (gpointer *) &info->regs;

	/* Notify the JIT */
	if (mono_gc_get_gc_callbacks ()->thread_suspend_func)
		mono_gc_get_gc_callbacks ()->thread_suspend_func (info->runtime_data, NULL);

	return TRUE;
}

void
mono_sgen_wait_for_suspend_ack (int count)
{
	/* Not needed for synchronous suspension */
}

int
mono_sgen_thread_handshake (BOOL suspend)
{
	SgenThreadInfo *info;
	pthread_t me;
	int count = 0;

	me = pthread_self ();

	FOREACH_THREAD_SAFE (info) {		
		pthread_t tid = mono_thread_info_get_tid (info);
		if (tid->id == me->id)
			continue;
		if (info->gc_disabled)
			continue;
		if (suspend) {
			g_assert (!info->doing_handshake);
			info->doing_handshake = TRUE;
			
			if (!mono_sgen_suspend_thread (info))
				continue;
		} else {
			g_assert (info->doing_handshake);
			info->doing_handshake = FALSE;

			if (!mono_sgen_resume_thread (info))
				continue;
		}

		++count;
	} END_FOREACH_THREAD_SAFE
	return count;
}

void
mono_sgen_os_init (void)
{
}

int
mono_gc_get_suspend_signal (void)
{
	return -1;
}
#endif
