#ifndef __EGLIB_CONFIG_H
#define __EGLIB_CONFIG_H

/*
 * System-dependent settings
 */
#define G_GNUC_PRETTY_FUNCTION   
#define G_GNUC_UNUSED            __attribute__((__unused__))
#define G_BYTE_ORDER             G_LITTLE_ENDIAN
#define G_GNUC_NORETURN          __attribute__((__noreturn__))
/* ':' is used in VITA paths */
#define G_SEARCHPATH_SEPARATOR_S ";"
#define G_SEARCHPATH_SEPARATOR   ';'
#define G_DIR_SEPARATOR          '/'
#define G_DIR_SEPARATOR_S        "/"
#define G_BREAKPOINT()           G_STMT_START { __asm__ ("bkpt"); } G_STMT_END
#define G_OS_UNIX
#define GPOINTER_TO_INT(ptr)   ((gint) (ptr))
#define GPOINTER_TO_UINT(ptr)  ((guint) (ptr))
#define GINT_TO_POINTER(v)     ((gpointer) (v))
#define GUINT_TO_POINTER(v)    ((gpointer) (v))

#if 1 == 1
#define G_HAVE_ALLOCA_H
#endif

typedef unsigned int gsize;
typedef signed   int gssize;
typedef int pid_t;

#define G_GSIZE_FORMAT   "u"
#define G_GUINT64_FORMAT "llu"
#define G_GINT64_FORMAT "lld"
#define G_GUINT32_FORMAT "u"
#define G_GINT32_FORMAT "d"

#if 1 == 1
#define G_HAVE_ISO_VARARGS
#endif

#if defined (__native_client__)
#define sem_trywait(x) sem_wait(x)
#define sem_timedwait(x,y) sem_wait(x)
#define getdtablesize() (32768)
#undef G_BREAKPOINT
#define G_BREAKPOINT()
#endif

typedef int GPid;

extern int vasprintf(char **ret, const char *fmt, va_list ap);

#endif
