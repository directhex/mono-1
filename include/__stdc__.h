#ifndef _PSS_STDC_H_
#define _PSS_STDC_H_

#ifdef __cplusplus
#error This header shouldn't be used in C++.
#endif

#define _EXTERN_C_BEGIN_
#define _EXTERN_C_END_


_EXTERN_C_BEGIN_

/*** assert.h ***/
#ifdef NDEBUG
 #define assert(test)	((void)0)
#else /* NDEBUG */
 #define _STRIZE(x)	_VAL(x)
 #define _VAL(x)	#x

 #if 199901L <= __STDC_VERSION__
  #define _FUNNAME	__func__
 #else /* 199901L <= __STDC_VERSION__ */
  #define _FUNNAME	0
 #endif /* 199901L <= __STDC_VERSION__ */

 #define assert(test)	((test) ? (void)0 \
	 : _CSTD _PSS_Assert(__FILE__ "(" _STRIZE(__LINE__) ") : Assertion Failed (" #test ")", _FUNNAME))

	void _PSS_Assert(const char *, const char *);
#endif /* NDEBUG */


/*** stdarg.h ***/
typedef char *  va_list;
extern char *__va_funcx (int, va_list, ...);

#define va_start(ap, N) ((ap) = __va_funcx (2, (char *) __builtin_addrof ((N))))
#define va_end(ap)      ((void) 0)
#define va_arg(ap, T)   (*((T *) (void *) __va_funcx (3, (ap), ((T *) 0))))

#if !defined (__STRICT_ANSI__) || __STDC_VERSION__ >= 199900L
#define va_copy(d, s)  ((d) = (s))
#endif


/*** yvals.h ***/
#define _C99 1
#define _HAS_IMMUTABLE_SETS           1 // 1 to permit alterable set elements
#define _HAS_TRADITIONAL_IOSTREAMS    0 // 1 to omit old iostreams functions
#define _HAS_TRADITIONAL_ITERATORS    0 // 1 for vector/string pointer iterators
#define _HAS_TRADITIONAL_POS_TYPE     0 // 1  for streampos same as streamoff
#define _HAS_TRADITIONAL_STL          0
#define _HAS_TR1

#define _MULTI_THREAD	1	/* 0 for no thread locks */
#define _COMPILER_TLS	1	/* 1 if compiler supports TLS directly */
#define _ADDED_C_LIB	1

typedef unsigned short int wchar_t;

#ifndef __STDC_HOSTED__
  #define __STDC_HOSTED__	1
#endif /* __STDC_HOSTED__ */

typedef int _Ptrdifft;
typedef unsigned int _Sizet;

#define _STD_BEGIN
#define _STD_END
#define _STD

#define _X_STD_BEGIN
#define _X_STD_END
#define _XSTD

#define _C_STD_BEGIN
#define _C_STD_END
#define _CSTD

#define _C_LIB_DECL
#define _END_C_LIB_DECL
#define _EXTERN_C
#define _END_EXTERN_C

#define _Restrict restrict
 
#define _LONGLONG	long long
#define _ULONGLONG	unsigned long long
#define _LLONG_MAX	0x7fffffffffffffffLL
#define _ULLONG_MAX	0xffffffffffffffffULL

#define _BITS_BYTE	8
#define _C2		1	/* 0 if not 2's complement */
#define _MBMAX		4	/* MB_LEN_MAX. UTF-8 is a standard multibyte stream format on PSS. */
#define _ILONG		1	/* 0 if 16-bit int */
#define _LLONG		0	/* 32-bit longs */
#define _HAS_C9X	1

typedef _LONGLONG _Longlong;
typedef _ULONGLONG _ULonglong;

typedef int _Int32t;
typedef unsigned int _Uint32t;

#define _WCMIN	0x0
#define _WCMAX	0xffff
#define _WIMAX	0xffffffffU

typedef unsigned short   _Wchart;
typedef unsigned int     _Wintt;

#define _NULL		0	/* 0L if pointer same as long */

typedef va_list _Va_list;

#define _FD_TYPE	int

#define _FILE_OFFSET_TYPE  signed long long 

#define _NO_RETURN(fun)	void fun __attribute__((__noreturn__))


/*** stddef.h ***/
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

typedef _Sizet size_t;
#define offsetof(T, member) ((_Sizet)__INTADDR__(&(((T *)0)->member)))
typedef _Ptrdifft ptrdiff_t;


/*** stdint.h ***/
typedef signed char int8_t;
typedef short int16_t;
typedef _Int32t int32_t;
typedef _Longlong int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef _Uint32t uint32_t;
typedef _ULonglong uint64_t;

typedef int intptr_t;
typedef unsigned int uintptr_t;

#define INT32_MIN	(-0x7fffffff - _C2)
#define INT32_MAX	0x7fffffff


/*** stdio.h ***/
#define BUFSIZ		(64*1024)
#define EOF			(-1)
#define FILENAME_MAX	_FNAMAX
#define FOPEN_MAX		_FOPMAX

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define stdin		(&_CSTD _Stdin)
#define stdout		(&_CSTD _Stdout)
#define stderr		(&_CSTD _Stderr)

typedef struct _Mbstatet
{	/* state of a multibyte translation */
	unsigned long _Wchar;
	unsigned short _Byte, _State;
} _Mbstatet;
typedef struct fpos_t
{	/* file position */
	_FILE_OFFSET_TYPE _Off; /* can be system dependent */
	_Mbstatet _Wstate;
} fpos_t;

typedef struct _Filet _Filet;
typedef struct _Filet FILE;

extern FILE _Stdin, _Stdout, _Stderr;

void clearerr(FILE *stream);
int fclose(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fflush(FILE *stream);
int fgetc(FILE *stream);
int fgetpos(FILE *_Restrict stream, fpos_t *_Restrict pos);
char *fgets(char *_Restrict s, int n, FILE *_Restrict stream);
FILE *fopen(const char *_Restrict filename, const char *_Restrict mode);

 #ifdef __SNC__
  #pragma __printf_args
 #endif /* __SNC__ */

int fprintf(FILE *_Restrict stream, const char *_Restrict format, ...);
int fputc(int c, FILE *stream);
int fputs(const char *_Restrict s, FILE *_Restrict stream);
size_t fread(void *_Restrict ptr, size_t size, size_t nelem, FILE *_Restrict stream);
FILE *freopen(const char *_Restrict filename, const char *_Restrict mode, FILE *_Restrict stream);

 #ifdef __SNC__
  #pragma __scanf_args
 #endif /* __SNC__ */

int fscanf(FILE *_Restrict stream, const char *_Restrict format, ...);
int fseek(FILE *stream, long offset, int mode);
int fsetpos(FILE *stream, const fpos_t *pos);
long ftell(FILE *stream);
size_t fwrite(const void *_Restrict ptr, size_t size, size_t nelem, FILE *_Restrict stream);
char *gets(char *s);
void perror(const char *s);

 #ifdef __SNC__
  #pragma __printf_args
 #endif /* __SNC__ */

int printf(const char *_Restrict format, ...);
int puts(const char *s);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);
void rewind(FILE *stream);

 #ifdef __SNC__
  #pragma __scanf_args
 #endif /* __SNC__ */

int scanf(const char *_Restrict format, ...);
void setbuf(FILE *_Restrict stream, char *_Restrict buf);
int setvbuf(FILE *_Restrict stream, char *_Restrict buf, int mode, size_t size);

 #ifdef __SNC__
  #pragma __printf_args
 #endif /* __SNC__ */

int sprintf(char *_Restrict s, const char *_Restrict format, ...);

 #ifdef __SNC__
  #pragma __scanf_args
 #endif /* __SNC__ */

int sscanf(const char *_Restrict s, const char *_Restrict format, ...);
int ungetc(int c, FILE *stream);
int vfprintf(FILE *_Restrict stream, const char *_Restrict format, _Va_list ap);
int vprintf(const char *_Restrict format, _Va_list ap);
int vsprintf(char *_Restrict s, const char *_Restrict format, _Va_list ap);

#if _ADDED_C_LIB
FILE *fdopen(_FD_TYPE fd, const char *mode);
int fileno(FILE *stream);
#endif /* _ADDED_C_LIB */

_FILE_OFFSET_TYPE _Fgpos(FILE *stream, fpos_t *pos);
int _Fspos(FILE *stream, const fpos_t *pos, _FILE_OFFSET_TYPE offset, int);

 #ifdef __SNC__
  #pragma __printf_args
 #endif /* __SNC__ */

int snprintf(char *_Restrict s, size_t n, const char *_Restrict format, ...);
int vsnprintf(char *_Restrict s, size_t n, const char *_Restrict format, _Va_list ap);
int vfscanf(FILE *_Restrict stream, const char *_Restrict format, _Va_list ap);
int vscanf(const char *_Restrict format, _Va_list ap);
int vsscanf(const char *_Restrict s, const char *_Restrict format, _Va_list ap);


/*** stdlib.h ***/
#define RAND_MAX 32767

typedef struct _div_t {
        int quot;
        int rem;
} div_t;

typedef struct {	/* result of long divide */
	long quot;
	long rem;
} ldiv_t;

div_t div(int, int);
ldiv_t ldiv(long, long);

_NO_RETURN(abort(void));
_NO_RETURN(exit(int));
_NO_RETURN(_Exit(int));

int atexit(void (*)(void));

extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern void free(void *);
extern void *calloc(size_t, size_t);

extern char *atob( char *s, int *i );
extern int atoi( const char * );
extern long atol( const char * );
double strtod(const char *_Restrict, char **_Restrict);
unsigned long strtoul(const char *_Restrict, char **_Restrict, int);
extern double atof( const char * );
extern int abs( int i );
extern long labs(long);
extern int rand( void );
extern void srand( unsigned int seed );
extern void qsort( void *base, size_t nmemb, size_t size,
	int(*compar)( const void *, const void * ) );
extern void *bsearch( const void *key, const void *base, size_t nmemb,
	size_t size, int (*compar)( const void *, const void * ) );
/* (string.c) */
extern long strtol( const char *, char **, int );
extern unsigned long strtoul( const char *, char **, int );
extern long long strtoll( const char *, char **, int );
extern unsigned long long strtoull( const char *, char **, int );


/*** setjmp.h ***/
#define _JBLEN (24)
typedef uint64_t jmp_buf[_JBLEN];


/*** string.h/strings.h ***/
extern char *strcat( char *dest, const char *src );
extern char *strncat( char *dest, const char *src, size_t n );
extern int strcmp( const char *s1, const char *s2 );
extern int strcasecmp( const char *s1, const char *s2 );
extern int strncmp( const char *s1, const char *s2, size_t n );
extern int strncasecmp( const char *s1, const char *s2, size_t n );
extern char *strdup(const char *s);
extern char *strcpy( char *dest, const char *src );
extern char *strncpy( char *dest, const char *src, size_t n );
extern size_t strlcpy( char *dest, const char *src, size_t n );
extern size_t strlen( const char *s );
extern char *index( const char *s, int c );
extern char *rindex( const char *s, int c );
extern char *strchr( const char *s, int c );
extern char *strrchr( const char *s, int c );
extern char *strpbrk( const char *s1, const char *s2 );
extern int strspn( const char *s1, const char *s2 );
extern int strcspn( const char *s1, const char *s2 );
extern char *strtok( char *s1, const char *s2 );
extern char *strtok_r( char *s1, const char *s2, char **lasts );
extern char *strstr( const char *s1, const char *s2 );
extern char *strerror(int errcode);
extern void *memchr( const void *p, int c, size_t n );
extern int memcmp( const void *s1, const void *s2, size_t n );
extern void *memcpy( void *dest, const void *src, size_t n );
extern void *memmove( void *dest, const void *src, size_t n );
extern void *vfpmemcpy( void *dest, const void *src, size_t n );
extern void *vfpmemmove( void *dest, const void *src, size_t n );
extern void *memset( void *s, int c, size_t n );
extern int bcmp( const void *s1, const void *s2, size_t n );
extern void bcopy( const void *src, void *dest, size_t n );
extern void bzero( void *s, size_t n );


/*** wchar.h ***/
extern wchar_t *wcscat( wchar_t *dest, const wchar_t *src );
extern wchar_t *wcsncat( wchar_t *dest, const wchar_t *src, size_t n );
extern int wcscmp( const wchar_t *s1, const wchar_t *s2 );
extern int wcscasecmp( const wchar_t *s1, const wchar_t *s2 );
extern int wcsncmp( const wchar_t *s1, const wchar_t *s2, size_t n );
extern int wcsncasecmp( const wchar_t *s1, const wchar_t *s2, size_t n );
extern wchar_t *wcscpy( wchar_t *dest, const wchar_t *src );
extern wchar_t *wcsncpy( wchar_t *dest, const wchar_t *src, size_t n );
extern size_t wcslen( const wchar_t *s );
extern wchar_t *wcschr( const wchar_t *s, int c );
extern wchar_t *wcsrchr( const wchar_t *s, int c );
extern wchar_t *wcspbrk( const wchar_t *s1, const wchar_t *s2 );
extern int wcsspn( const wchar_t *s1, const wchar_t *s2 );
extern int wcscspn( const wchar_t *s1, const wchar_t *s2 );
extern wchar_t *wcstok( wchar_t *s1, const wchar_t *s2, wchar_t **lasts );
extern wchar_t *wcsstr( const wchar_t *s1, const wchar_t *s2 );
extern wchar_t *wmemchr( const wchar_t *p, wchar_t c, size_t n );
extern int wmemcmp( const wchar_t *s1, const wchar_t *s2, size_t n );
extern wchar_t *wmemcpy( wchar_t *dest, const wchar_t *src, size_t n );
extern wchar_t *wmemmove( wchar_t *dest, const wchar_t *src, size_t n );
extern wchar_t *wmemset( wchar_t *s, wchar_t c, size_t n );
extern int swprintf( wchar_t *, size_t, const wchar_t *, ... );
extern int vswprintf( wchar_t *, size_t, const wchar_t *, va_list );
extern int wprintf( const wchar_t *, ... );


/*** ctype.h ***/
int isalnum(int c);
int isalpha(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int tolower(int c);
int toupper(int c);


/*** float.h ***/
#define DBL_DIG			15
#define DBL_MAX_10_EXP	308
#define DBL_MAX_EXP		1024
#define FLT_RADIX		2


/*** ymath.h ***/
#define _DENORM		(-2)	/* C9X only */
#define _FINITE		(-1)
#define _INFCODE	1
#define _NANCODE	2

double _Cosh(double, double);
short _Dtest(double *);
short _Exp(double *, double, long);
double _Log(double, int);
double _Sin(double, unsigned int);
double _Sinh(double, double);


/*** math.h? ***/
#define M_E		2.7182818284590452354f
#define M_PI	3.14159265358979323846f
#define M_PI_2	1.57079632679489661923f


/*** math.h ***/
int _FDclass(float);
int _Dclass(double);
int _LDclass(long double);

int _FDsign(float);
int _Dsign(double);
int _LDsign(long double);

#define FP_INFINITE		_INFCODE
#define FP_NAN			_NANCODE
#define FP_NORMAL		_FINITE
#define FP_SUBNORMAL	_DENORM
#define FP_ZERO			0

   #define _ARG(x)	(sizeof ((x) + (float)0) == sizeof (float) ? 'f' \
	: sizeof ((x) + (double)0) == sizeof (double) ? 'd' \
	: 'l')
   #define _CARGI(x, fd, ff, fl)	\
	(_ARG(x) == 'f' ? ff((float)(x)) \
	: _ARG(x) == 'd' ? fd((double)(x)) \
	: fl((long double)(x)))
   #define _CARG2I(x, y, fd, ff, fl)	\
	(_ARG((x) + (y)) == 'f' ? ff((float)(x), (float)(y)) \
	: _ARG((x) + (y)) == 'd' ? fd((double)(x), (double)(y)) \
	: fl((long double)(x), (long double)(y)))

  #define _FPCOMP(x, y) \
	_CARG2I(x, y, _Fpcomp, _FFpcomp, _LFpcomp)
  #define fpclassify(x) \
	_CARGI(x, _Dclass, _FDclass, _LDclass)
  #define signbit(x) \
	_CARGI(x, _Dsign, _FDsign, _LDsign)

#define isfinite(x)	(fpclassify(x) <= 0)
#define isinf(x)	(fpclassify(x) == FP_INFINITE)
#define isnan(x)	(fpclassify(x) == FP_NAN)
#define isnormal(x)	(fpclassify(x) == FP_NORMAL)

#define isgreater(x, y)	((_FPCOMP(x, y) & _FP_GT) != 0)
#define isgreaterequal(x, y)	\
	((_FPCOMP(x, y) & (_FP_EQ | _FP_GT)) != 0)
#define isless(x, y)	((_FPCOMP(x, y) & _FP_LT) != 0)
#define islessequal(x, y)	((_FPCOMP(x, y) & (_FP_LT | _FP_EQ)) != 0)
#define islessgreater(x, y)	\
	((_FPCOMP(x, y) & (_FP_LT | _FP_GT)) != 0)
#define isunordered(x, y)	(_FPCOMP(x, y) == 0)

	/* double declarations */
double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double ceil(double x);
double exp(double x);
double fabs(double x);
double floor(double x);
double fmod(double x, double y);
double frexp(double x, int *pexp);
double ldexp(double x, int ex);
double modf(double x, double *pint);
double pow(double x, double y);
double sqrt(double x);
double tan(double x);
double tanh(double x);

 #if _HAS_C9X
double acosh(double x);
double asinh(double x);
double atanh(double x);
double cbrt(double x);
double copysign(double x, double y);
double erf(double x);
double erfc(double x);
double exp2(double x);
double expm1(double x);
double fdim(double x, double y);
double fma(double x, double y, double z);
double fmax(double x, double y);
double fmin(double x, double y);
double hypot(double x, double y);
int ilogb(double x);
double lgamma(double x);
_Longlong llrint(double x);
_Longlong llround(double x);
double log1p(double x);
double logb(double x);
long lrint(double x);
long lround(double x);
double nan(const char *str);
double nearbyint(double x);
double nextafter(double x, double y);
double nexttoward(double x, long double y);
double remainder(double x, double y);
double remquo(double x, double y, int *pquo);
double rint(double x);
double round(double x);
double scalbn(double x, int ex);
double scalbln(double x, long ex);
double tgamma(double x);
double trunc(double x);
 #endif /* _IS_C9X */

	/* float declarations */
float acosf(float x);
float asinf(float x);
float atanf(float x);
float atan2f(float y, float x);
float ceilf(float x);
float expf(float x);
float fabsf(float x);
float floorf(float x);
float fmodf(float x, float y);
float frexpf(float x, int *pexp);
float ldexpf(float x, int ex);
float modff(float x, float *pint);
float powf(float x, float y);
float sqrtf(float x);
float tanf(float x);
float tanhf(float x);

 #if _HAS_C9X
float acoshf(float x);
float asinhf(float x);
float atanhf(float x);
float cbrtf(float x);
float copysignf(float x, float y);
float erff(float x);
float erfcf(float x);
float expm1f(float x);
float exp2f(float x);
float fdimf(float x, float y);
float fmaf(float x, float y, float z);
float fmaxf(float x, float y);
float fminf(float x, float y);
float hypotf(float x, float y);
int ilogbf(float x);
float lgammaf(float x);
_Longlong llrintf(float x);
_Longlong llroundf(float x);
float log1pf(float x);
float logbf(float x);
long lrintf(float x);
long lroundf(float x);
float nanf(const char *str);
float nearbyintf(float x);
float nextafterf(float x, float y);
float nexttowardf(float x, long double y);
float remainderf(float x, float y);
float remquof(float x, float y, int *pquo);
float rintf(float x);
float roundf(float x);
float scalbnf(float x, int ex);
float scalblnf(float x, long ex);
float tgammaf(float x);
float truncf(float x);
 #endif /* _IS_C9X */


	/* long double declarations */
long double acosl(long double x);
long double asinl(long double x);
long double atanl(long double x);
long double atan2l(long double y, long double x);
long double ceill(long double x);
long double expl(long double x);
long double fabsl(long double x);
long double floorl(long double x);
long double fmodl(long double x, long double y);
long double frexpl(long double x, int *pexp);
long double ldexpl(long double x, int ex);
long double modfl(long double x, long double *pint);
long double powl(long double x, long double y);
long double sqrtl(long double x);
long double tanl(long double x);
long double tanhl(long double x);

 #if _HAS_C9X
long double acoshl(long double x);
long double asinhl(long double x);
long double atanhl(long double x);
long double cbrtl(long double x);
long double copysignl(long double x, long double y);
long double erfl(long double x);
long double erfcl(long double x);
long double exp2l(long double x);
long double expm1l(long double x);
long double fdiml(long double x, long double y);
long double fmal(long double x, long double y, long double z);
long double fmaxl(long double x, long double y);
long double fminl(long double x, long double y);
long double hypotl(long double x, long double y);
int ilogbl(long double x);
long double lgammal(long double x);
_Longlong llrintl(long double x);
_Longlong llroundl(long double x);
long double log1pl(long double x);
long double logbl(long double x);
long lrintl(long double x);
long lroundl(long double x);
long double nanl(const char *str);
long double nearbyintl(long double x);
long double nextafterl(long double x, long double y);
long double nexttowardl(long double x, long double y);
long double remainderl(long double x, long double y);
long double remquol(long double x, long double y, int *pquo);
long double rintl(long double x);
long double roundl(long double x);
long double scalbnl(long double x, int ex);
long double scalblnl(long double x, long ex);
long double tgammal(long double x);
long double truncl(long double x);
 #endif /* _IS_C9X */


	/* double MACRO OVERRIDES, FOR C */
double cos(double x);
double cosh(double x);
double log(double x);
double log10(double x);
double sin(double x);
double sinh(double x);

 #define cos(x)		_Sin(x, 1)
 #define cosh(x)	_Cosh(x, 1)
 #define log(x)		_Log(x, 0)
 #define log10(x)	_Log(x, 1)
 #define sin(x)		_Sin(x, 0)
 #define sinh(x)	_Sinh(x, 1)

 #if _HAS_C9X
double log2(double x);

 #define log2(x)	_Log(x, -1)
 #endif /* _IS_C9X */

	/* float MACRO OVERRIDES, FOR C */
float cosf(float x);
float coshf(float x);
float logf(float x);
float log10f(float x);
float sinf(float x);
float sinhf(float x);

 #define cosf(x)	_FSin(x, 1)
 #define coshf(x)	_FCosh(x, 1)
 #define logf(x)	_FLog(x, 0)
 #define log10f(x)	_FLog(x, 1)
 #define sinf(x)	_FSin(x, 0)
 #define sinhf(x)	_FSinh(x, 1)

 #if _HAS_C9X
float log2f(float);

 #define log2f(x)	_FLog(x, -1)
 #endif /* _IS_C9X */

	/* long double MACRO OVERRIDES, FOR C */
long double cosl(long double x);
long double coshl(long double x);
long double logl(long double x);
long double log10l(long double x);
long double sinl(long double x);
long double sinhl(long double x);

 #define cosl(x)	_LSin(x, 1)
 #define coshl(x)	_LCosh(x, 1)
 #define logl(x)	_LLog(x, 0)
 #define log10l(x)	_LLog(x, 1)
 #define sinl(x)	_LSin(x, 0)
 #define sinhl(x)	_LSinh(x, 1)

 #if _HAS_C9X
long double log2l(long double x);

 #define log2l(x)	_LLog(x, -1)
 #endif /* _IS_C9X */


/*** errno.h ***/
#define ENOENT        0x0002

#define EDOM          0x0021
#define EFAULT        0x000E
#define EFBIG         0x001B
#define EFPOS         0x0098
#define EMLINK        0x001F
#define ENFILE        0x0017
#define ENOENT        0x0002
#define ENOSPC        0x001C
#define ENOTTY        0x0019
#define EPIPE         0x0020
#define ERANGE        0x0022
#define EROFS         0x001E
#define ESPIPE        0x001D
#define E2BIG         0x0007
#define EACCES        0x000D
#define EAGAIN        0x000B
#define EBADF         0x0009
#define EBUSY         0x0010
#define ECHILD        0x000A
#define EEXIST        0x0011
#define EINTR         0x0004
#define EINVAL        0x0016
#define EIO           0x0005
#define EISDIR        0x0015
#define EMFILE        0x0018
#define ENODEV        0x0013
#define ENOEXEC       0x0008
#define ENOMEM        0x000C
#define ENOTDIR       0x0014
#define ENXIO         0x0006
#define EPERM         0x0001
#define ESRCH         0x0003
#define EXDEV         0x0012
#define EBADMSG       0x004D
#define ECANCELED     0x008C
#define EDEADLK       0x002D
#define EILSEQ        0x008A
#define EINPROGRESS   0x0077
#define EMSGSIZE      0x007A
#define ENAMETOOLONG  0x005B
#define ENOLCK        0x002E
#define ENOSYS        0x0058
#define ENOTEMPTY     0x005A
#define ENOTSUP       0x0086
#define ETIMEDOUT     0x0074

int *_PSSLibcErrnoLoc(void);
#define errno (*_CSTD _PSSLibcErrnoLoc())


/*** limits.h ***/
  #if _LLONG
   #define LONG_MAX		0x7fffffffffffffffL
   #define LONG_MIN		(-LONG_MAX - _C2)
   #define ULONG_MAX	0xffffffffffffffffUL

  #else /* _LLONG */
   #define LONG_MAX		0x7fffffffL
   #define LONG_MIN		(-LONG_MAX - _C2)
   #define ULONG_MAX	0xffffffffUL
  #endif /* _LLONG */

  #define SHRT_MAX	0x7fff
  #define SHRT_MIN	(-SHRT_MAX - _C2)

  #if _ILONG
   #define INT_MAX		0x7fffffff
   #define INT_MIN		(-INT_MAX - _C2)
   #define UINT_MAX		0xffffffffU

  #else /* _ILONG */
   #define INT_MAX		SHRT_MAX
   #define INT_MIN		SHRT_MIN
   #define UINT_MAX		USHRT_MAX
  #endif /* _ILONG */


/*** time.h ***/
typedef unsigned int time_t;

 #ifndef _TM_DEFINED
  #define _TM_DEFINED
struct tm
	{	/* date and time components */
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	};
 #endif /* _TM_DEFINED */

time_t time(time_t *tod);
double difftime(time_t t1, time_t t0);
struct tm *gmtime(const time_t *tod);
struct tm *localtime(const time_t *tod);
time_t mktime(struct tm *tptr);
size_t strftime(char *_Restrict s, size_t n, const char *_Restrict format, const struct tm *_Restrict tptr);


/*** alloca.h ***/
extern void * alloca (size_t __size);


/*** threadmgr.h ***/
#define PSS_THREAD_ATTR_TH_FIFO				(0x00000000U)
#define PSS_THREAD_ATTR_TH_PRIO				(0x00002000U)
#define PSS_THREAD_EVENT_WAIT_MODE_OR		(0x00000001U)
#define PSS_THREAD_EVENT_WAIT_MODE_AND		(0x00000002U)
#define PSS_THREAD_EVF_WAITMODE_CLEAR_ALL	(0x00000002U)
#define PSS_THREAD_EVF_WAITMODE_CLEAR_PAT	(0x00000004U)


_EXTERN_C_END_

#endif // _PSS_STDC_H_
