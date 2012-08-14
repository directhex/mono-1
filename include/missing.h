#ifndef __MISSING_H__
#define __MISSING_H__

#define _POSIX_PATH_MAX 1024

extern int setenv(const char *variable, const char *value, int overwrite);

extern const char* getenv(const char* pVar);

extern char *setlocale(int category,  const char *locale);

extern int system(const char *command);

extern int getpid();

extern int isascii(int c);

#include <sys/types.h>

#define __builtin_return_address(x) __builtin_return_address()

#endif
