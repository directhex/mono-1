#ifndef __SIGNAL_H__
#define __SIGNAL_H__

/* The numbers doesn't matter */
#define SIGSEGV         11      /* segment violation */
#define SIGUSR1         12

typedef struct siginfo_t {
	int si_signo;
	char *si_addr;
} siginfo_t;

#endif /* __SIGNAL_H__ */
