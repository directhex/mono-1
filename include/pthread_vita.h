//Sony Computer Entertainment Confidential
#ifndef _H_pthread_vita
#define _H_pthread_vita

#include <kernel.h>
#include <stdlib.h>
#include <errno.h>

#if defined(__cplusplus)
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

typedef struct sched_param sched_param;

/*** constant.h ***/
#define PSS_UID_NAMELEN		(31)

#define _THREAD_SAFE 

/* Most compiler do not support the restrict keyword */
#define restrict


/*
 * POSIX 1003.1c-1995 options / 1003.1 2001 options
 */

#define _POSIX_THREADS                          2001L   // We can use threads
#define _POSIX_THREAD_ATTR_STACKSIZE            2001L   // Stack size is supported
#define _POSIX_THREAD_PRIO_INHERIT              2001L   // Priority inheritance mutexes supported
#define _POSIX_THREAD_PRIO_PROTECT              2001L   // Priority ceiling mutexes supported
#define _POSIX_READER_WRITER_LOCKS              2001L   // Multiple readers / single writer
#define _POSIX_SPIN_LOCKS                       2001L   // Spin lock
#define _POSIX_BARRIERS                         2001L   // Barriers
#define _POSIX_THREAD_PRIORITY_SCHEDULING       2001L   // Realtime scheduling is supported

/* 
 * The following features are not supported
 */

#define _POSIX_THREAD_ATTR_STACKADDR            -1      // Stack addr is supported
#define _POSIX_SEMAPHORES                       -1      // Old style semaphores
#define _POSIX_THREAD_PROCESS_SHARED            -1      // Mutexes and conditions can be shared across processes
#define _POSIX_THREAD_SAFE_FUNCTIONS            -1      // "_r" functions are supported

typedef int		int32_t;

// int32_t values
#define INVALID_ID_          ((int32_t)(-1))
#define STATIC_INIT_ID_      ((int32_t)(-2))
#define DELETED_ID_          ((int32_t)(-3))


/* ************************************* */
/* ********** PThreads Limits ********** */
/* ************************************* */
 
/** @defgroup Limits PThreads Limits
 *
 * @{
 */

/*
 * POSIX 1003.1c-1995 limits
 */

/**
 * Maximum number of attempts to destroy a thread's thread-specific data (>= 4)
 */
#define PTHREAD_DESTRUCTOR_ITERATIONS   4       
                                                

/**
 * Maximum number of thread-specific data keys per process (>= 128), but we slim that down below spec
 */
#define PTHREAD_KEYS_MAX                PTHREAD_KEYS_MAX_


/**
 * Minimum stack size 
 * Note: It seems that 4K is the minimum stack required to run any Boost tests
 */
#define PTHREAD_STACK_MIN               (4*1024)


/**
 * Maximum number of threads per process (>= 64)
 */
#define PTHREAD_THREADS_MAX             128     

/** @} */


#define _POSIX_THREAD_THREADS_MAX               PTHREAD_THREADS_MAX
#define _POSIX_THREAD_KEYS_MAX                  PTHREAD_KEYS_MAX
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS     PTHREAD_DESTRUCTOR_ITERATIONS

// Signatures used by static initializers
#define RWLOCK_SIG_           1
#define MUTEX_SIG_            2
#define COND_SIG_             3

#define PTHREAD_MUTEX_INITIALIZER_              { STATIC_INIT_ID_, (pthread_t)MUTEX_SIG_, 0, 0, (char)PTHREAD_MUTEX_NORMAL,     0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} }
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_    { STATIC_INIT_ID_, (pthread_t)MUTEX_SIG_, 0, 0, (char)PTHREAD_MUTEX_RECURSIVE,  0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} }
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_   { STATIC_INIT_ID_, (pthread_t)MUTEX_SIG_, 0, 0, (char)PTHREAD_MUTEX_ERRORCHECK, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} }

#if defined(__cplusplus)
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif



/** 
 * Make sure we do not use a value that would be interpreted as an 
 * errno number
 */
#define PTHREAD_BARRIER_SERIAL_THREAD   (-2)


/** 
 * Just need a unique address 
 */
#define PTHREAD_CANCELED                ((void *) pthread_cancel)


/* **************************************** */
/* ********** Static Initializer ********** */
/* **************************************** */
 
/** @defgroup Static Static Initializers
 *
 * @{
 */
#define PTHREAD_ONCE_INIT                       PTHREAD_ONCE_INIT_

#define PTHREAD_MUTEX_INITIALIZER               PTHREAD_MUTEX_INITIALIZER_
#define PTHREAD_COND_INITIALIZER                PTHREAD_COND_INITIALIZER_
#define PTHREAD_RWLOCK_INITIALIZER              PTHREAD_RWLOCK_INITIALIZER_
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER     PTHREAD_RECURSIVE_MUTEX_INITIALIZER_
#define PTHREAD_SPINLOCK_INITIALIZER            PTHREAD_SPINLOCK_INITIALIZER_
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER    PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_

/** @} */


/* ************************************** */
/* ********** Type declaration ********** */
/* ************************************** */

struct timespec
{
  int tv_sec;
  int tv_nsec;
};

typedef struct timespec timespec;

/* ***************************** */
/* ********** Threads ********** */
/* ***************************** */

/** @defgroup Threads Threads
 *
 * @{
 */

enum {
  PTHREAD_PROCESS_PRIVATE,      // No support for shared
};


/* Declaration of a control to be put inside a pthread struct   */
#define CONTROL   int32_t control

typedef struct pthread_cleanup_t
{
  void                      (*func)(void *);
  void                      *arg;
  struct pthread_cleanup_t  *next;
} pthread_cleanup_t;

// Number of per-thread keys that we support
#define PTHREAD_KEYS_MAX_     48



typedef struct pthread_storage_t
{
  CONTROL;                              // Lock control
  int32_t                id;             // ID of the thread
  int32_t                condCBID;       // Callback for support of cond variables
  int32_t                barrierCBID;    // Callback for support of barriers
  int32_t                joinCBID;       // Callback for support of join
  pthread_cleanup_t    *cleanup;
  volatile long         cancel_state;
  volatile long         cancel_type;
  int                   waiting;        // Number of threads waiting to join this thread
  unsigned long         detached;
  void                 *returncode;     // Thread return code
  char                  name[PSS_UID_NAMELEN+1];
  char                  cancel_pending;
  char                  joinable;
  char                  inWAIT;         // Indicates the thread is waiting for a pthread mutex or cond
  char                  terminated;     // Thread is terminated
  char                  needsfree;      // Thread storage belongs to pthread lib and will be freed with PTHREAD_FREE

  unsigned char priority;

  unsigned char priomutex[128];
  
  /* Specific data has been moved inline instead of being dynamically allocated when accessed */
  int                   specific_data_count;
  const void           *specific_data[PTHREAD_KEYS_MAX_];
  
#ifdef __cplusplus
  /* Helper operators for C++ */
  PTHREAD_CPP_OPERATORS(pthread_storage_t, id)
  inline operator struct pthread_storage_t * () { return this; }
  inline operator int32_t () { return id; }
#endif
} pthread_storage_t;
typedef pthread_storage_t* pthread_t;


typedef struct pthread_attr_t
{
  int joinable;
  size_t stacksize;
  int priority;
  unsigned int attr;
  char name[PSS_UID_NAMELEN - 9 + 1];
  pthread_storage_t *storage;
} pthread_attr_t;


EXTERN int pthread_attr_init(pthread_attr_t *attr);
EXTERN int pthread_attr_destroy(pthread_attr_t *attr);


enum {
  PTHREAD_CREATE_DETACHED,
  PTHREAD_CREATE_JOINABLE,
};

EXTERN int pthread_attr_setdetachstate(pthread_attr_t *attr, int state);

EXTERN int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *state);


/*
 * The useVFPU functions are used to indicate that the VFPU is used
 */

/*
EXTERN int pthread_attr_setuseVFPU_np(pthread_attr_t *attr, char vfpu);
EXTERN int pthread_attr_getuseVFPU_np(const pthread_attr_t *attr, char *vfpu);
*/

EXTERN int pthread_attr_setname_np(pthread_attr_t *attr, const char *name);
EXTERN int pthread_attr_getname_np(const pthread_attr_t *attr, char *name);


EXTERN int pthread_getschedparam(pthread_t thread,
                                 int *restrict policy,
                                 struct sched_param *restrict param);

EXTERN int pthread_setschedparam(pthread_t thread,
                                 int policy,
                                 const struct sched_param *restrict param);


EXTERN int pthread_attr_getschedparam(const pthread_attr_t *restrict attr,
                                      struct sched_param *restrict param);
EXTERN int pthread_attr_setschedparam(pthread_attr_t *restrict attr, 
                                      const struct sched_param *restrict param);

EXTERN int pthread_setschedprio(pthread_t thread, int prio);

EXTERN int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

EXTERN int pthread_attr_getstacksize(const pthread_attr_t *restrict attr, 
                                     size_t *restrict stacksize);

EXTERN int pthread_attr_setstorage_np(pthread_attr_t *attr, pthread_storage_t *storage);
EXTERN int pthread_attr_getstorage_np(pthread_attr_t *attr, pthread_storage_t **storage);

EXTERN int pthread_create(pthread_t *restrict thread, 
                          const pthread_attr_t *restrict attr,
                          void *(*start)(void *), 
                          void *restrict param);

EXTERN int pthread_detach(pthread_t thread);
EXTERN int pthread_equal(pthread_t t1, pthread_t t2);
EXTERN void pthread_exit(void *status);

EXTERN int pthread_join(pthread_t thread, void **value_ptr);

#ifdef __cplusplus
#define PTHREAD_CPP_OPERATORS(T,id) \
	inline T() {} \
	inline T(int rhs)                  { *this = rhs; } \
	inline T(void *rhs)                { *this = rhs; } \
	inline T& operator= (int rhs)      { if (rhs==0) id=INVALID_ID_; return *this; } \
	inline T& operator= (void *rhs)    { if (rhs==0) id=INVALID_ID_; return *this; } \
	inline bool operator== (int rhs)   { return (rhs==0) && (id==INVALID_ID_); } \
	inline bool operator== (void *rhs) { return (rhs==0) && (id==INVALID_ID_); } \
	inline bool operator!= (int rhs)   { return !(*this == rhs); } \
	inline bool operator!= (void *rhs) { return !(*this == rhs); } \
	inline operator bool()             { return (*this != 0); }
#else
#define PTHREAD_CPP_OPERATORS(T,id)
#endif


typedef struct pthread_mutex_t
{
  int32_t        id;
  pthread_t     owner;          // The pthread owning the lock
  int           recursivecount; // Recursive mutex only
  int           prioceiling;    // Priority ceiling
  char          type;
  char          protocol;       // PTHREAD_PRIO_*
  unsigned char priowait[128];

  PTHREAD_CPP_OPERATORS(pthread_mutex_t,id)
} pthread_mutex_t;


EXTERN pthread_t pthread_self(void);

#define pthread_cleanup_push(routine, arg) \
	do { pthread_cleanup_t pthread_cleanup_storage_; pthread_cleanup_push_(routine, arg, &pthread_cleanup_storage_)

#define pthread_cleanup_pop(execute) \
	pthread_cleanup_pop_(execute); } while(0)

EXTERN void pthread_cleanup_push_(void (*routine)(void *), void *arg, pthread_cleanup_t *pStorage);
EXTERN void pthread_cleanup_pop_(int execute);

/**
 * Cancel states
 */
enum {
  PTHREAD_CANCEL_ENABLE,
  PTHREAD_CANCEL_DISABLE,
};


/**
 * Cancel types
 */
enum {
  PTHREAD_CANCEL_ASYNCHRONOUS,
  PTHREAD_CANCEL_DEFERRED,
};

EXTERN int pthread_setcancelstate(int state, int *oldstate);

EXTERN int pthread_setcanceltype(int type, int *oldtype);

EXTERN void pthread_testcancel(void);

EXTERN int pthread_cancel(pthread_t thread);

/** @} */


/* ****************************************** */
/* ********** Thread specific data ********** */
/* ****************************************** */

/** @defgroup Tss Thread specific data
 *
 * @{
 */
typedef struct pthread_key_t
{
  int key;
} pthread_key_t;



EXTERN int pthread_key_create(pthread_key_t *key, void (*dtor)(void *));

EXTERN int pthread_key_delete(pthread_key_t key);

EXTERN void *pthread_getspecific(pthread_key_t key);

EXTERN int pthread_setspecific(pthread_key_t key, const void *value);

/* mono extension to get the value of a TLS key for a specific thread */
EXTERN void* pthread_getspecific_for_thread(pthread_t thread, pthread_key_t key);

/** @} */


/* ****************************** */
/* ********** Barriers ********** */
/* ****************************** */

/** @defgroup Barriers Barriers
 *
 * @{
 */
typedef struct pthread_barrier_t
{
  CONTROL;
  int32_t        queue;
  long          neededcount;
  long          count;

  PTHREAD_CPP_OPERATORS(pthread_barrier_t,control)
} pthread_barrier_t;


typedef struct pthread_barrierattr_t
{
} pthread_barrierattr_t;



EXTERN int pthread_barrier_init(pthread_barrier_t *restrict barrier, 
                                const pthread_barrierattr_t *restrict barrierattr, 
                                unsigned int count);
EXTERN int pthread_barrier_destroy(pthread_barrier_t *barrier);
EXTERN int pthread_barrier_wait(pthread_barrier_t *barrier);

EXTERN int pthread_barrierattr_destroy(pthread_barrierattr_t *barrierattr);
EXTERN int pthread_barrierattr_init(pthread_barrierattr_t *barrierattr);

/** @} */


/* ***************************************** */
/* ********** Condition Variables ********** */
/* ***************************************** */

/** @defgroup Conditions Condition Variables
 *
 * @{
 */
typedef struct pthread_cond_t
{
  CONTROL;
  int             NbWait;       // Number of threads waiting
  int32_t          lock;         // Sema used to queue the threads

  PTHREAD_CPP_OPERATORS(pthread_cond_t,control)
} pthread_cond_t;

#define PTHREAD_COND_INITIALIZER_               { STATIC_INIT_ID_, (int)COND_SIG_, 0 }

typedef struct pthread_condattr_t
{
} pthread_condattr_t;



EXTERN int pthread_cond_init(pthread_cond_t *restrict cond,
                             const pthread_condattr_t *restrict attr);
EXTERN int pthread_cond_broadcast(pthread_cond_t *cond);
EXTERN int pthread_cond_destroy(pthread_cond_t *cond);
EXTERN int pthread_cond_signal(pthread_cond_t *cond);
EXTERN int pthread_cond_timedwait(pthread_cond_t *restrict cond, 
                                  pthread_mutex_t *restrict mutex, 
                                  const struct timespec *restrict abstime);
EXTERN int pthread_cond_wait(pthread_cond_t *restrict cond, 
                             pthread_mutex_t *restrict mutex);

EXTERN int pthread_condattr_destroy(pthread_condattr_t *attr);
EXTERN int pthread_condattr_init(pthread_condattr_t *attr);

/** @} */


/* ***************************** */
/* ********** Mutexes ********** */
/* ***************************** */

/** @defgroup Mutexes Mutexes
 *
 * @{
 */

typedef struct pthread_mutexattr_t
{
  int MaxCount;
  int scheduling;               // PTHREAD_QUEUE_*
  int prioceiling;              // Priority ceiling
  char protocol;                // PTHREAD_PRIO_*
  char type;
} pthread_mutexattr_t;

EXTERN int pthread_mutex_destroy(pthread_mutex_t *mutex);
EXTERN int pthread_mutex_init(pthread_mutex_t *restrict mutex, 
                              const pthread_mutexattr_t *restrict attr);
EXTERN int pthread_mutex_lock(pthread_mutex_t *mutex);
EXTERN int pthread_mutex_trylock(pthread_mutex_t *mutex);
EXTERN int pthread_mutex_unlock(pthread_mutex_t *mutex);



EXTERN int pthread_mutex_timedlock(pthread_mutex_t *mutex, 
                                   const struct timespec *abstime);



EXTERN int pthread_mutex_getprioceiling(const pthread_mutex_t *restrict mutex, 
                                        int *restrict prioceiling);
EXTERN int pthread_mutex_setprioceiling(pthread_mutex_t *restrict mutex, 
                                        int prioceiling, 
                                        int *restrict old_ceiling);

EXTERN int pthread_mutexattr_init(pthread_mutexattr_t *attr);
EXTERN int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

enum {

#define PTHREAD_MUTEX_DEFAULT   PTHREAD_MUTEX_NORMAL

  PTHREAD_MUTEX_NORMAL,

  PTHREAD_MUTEX_ERRORCHECK,

  PTHREAD_MUTEX_RECURSIVE,
};

EXTERN int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
EXTERN int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);


enum {
  PTHREAD_PRIO_NONE,
  PTHREAD_PRIO_INHERIT,
  PTHREAD_PRIO_PROTECT,
};

EXTERN int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, 
                                         int *protocol);
EXTERN int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, 
                                         int protocol);


/**
 * Gets and sets the prioceiling attribute of the mutex attributes object.
 *
 * The pthread_mutexattr_getprioceiling and pthread_mutexattr_setprioceiling 
 * subroutines can fail if:
 *
 *   EINVAL:    The value specified by the attr or prioceiling parameter is 
 *              invalid. 
 *   ENOSYS:    This function is not supported (draft 7). 
 *   ENOTSUP:   This function is not supported together with checkpoint/restart. 
 *   EPERM:     The caller does not have the privilege to perform the operation. 
 */

EXTERN int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, 
                                            int *prioceiling);
EXTERN int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, 
                                            int prioceiling);


/** @} */


/* ************************************** */
/* ********** Read/Write Locks ********** */
/* ************************************** */

/** @defgroup ReadWrite Read/Write Locks
 *
 * @{
 */

typedef struct pthread_rwlock_t
{
  int32_t        rdwrQ;          // Readers and writers queue
  int           writelock;      // A writer has the control

  PTHREAD_CPP_OPERATORS(pthread_rwlock_t,rdwrQ)
} pthread_rwlock_t;

#define PTHREAD_RWLOCK_INITIALIZER_             { STATIC_INIT_ID_, (int)RWLOCK_SIG_ }

typedef struct pthread_rwlockattr_t
{
} pthread_rwlockattr_t;

EXTERN int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                               const pthread_rwlockattr_t *rwlockattr);


EXTERN int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

EXTERN int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);

EXTERN int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock, 
                                      const struct timespec *restrict abstime);

EXTERN int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);


EXTERN int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);


EXTERN int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);

EXTERN int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock, 
                                      const struct timespec *restrict abstime);
EXTERN int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

EXTERN int pthread_rwlockattr_destroy(pthread_rwlockattr_t *rwlockattr);
EXTERN int pthread_rwlockattr_init(pthread_rwlockattr_t *rwlockattr);

/** @} */



/* ******************************** */
/* ********** Spin Locks ********** */
/* ******************************** */

/** @defgroup Spin Spin Locks
 *
 * @{
 */
typedef struct pthread_spinlock_t
{
  unsigned long lock;
} pthread_spinlock_t;

#define PTHREAD_SPINLOCK_INITIALIZER_           { 0 }

EXTERN int pthread_spin_destroy(pthread_spinlock_t *lock);
EXTERN int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
EXTERN int pthread_spin_lock(pthread_spinlock_t *lock);
EXTERN int pthread_spin_trylock(pthread_spinlock_t *lock);
EXTERN int pthread_spin_unlock(pthread_spinlock_t *lock);

/** @} */


/* ******************************************* */
/* ********** Mailbox support (_np) ********** */
/* ******************************************* */

/** @defgroup Mbx Mailbox
 *
 * @{
 */

/*
 * The mailbox mechanism is used to pass messages between
 * thread.  The messages are passed by reference, that is 
 * a pointer to the message is passed to the receiving 
 * thread.
 *
 * The pthread_mbx* functions returns similar return code than 
 * the corresponding standard functions in pthread.
 */

typedef struct pthread_mbxmsg_t
{
  char data[0]; // Place the data after the header
} pthread_mbxmsg_t;


typedef struct pthread_mbx_t
{
  int32_t id;
} pthread_mbx_t;


typedef struct pthread_mbxattr_t
{
  int qwait;
  int qmsg;
} pthread_mbxattr_t;


typedef struct pthread_msgpipe_t
{
  int32_t id;

  PTHREAD_CPP_OPERATORS(pthread_msgpipe_t,id)
} pthread_msgpipe_t;


typedef struct pthread_msgpipeattr_t
{
  int attr;
  int bufsize;
  int partition;
} pthread_msgpipeattr_t;


typedef struct pthread_eventflag_t
{
  int32_t id;

  PTHREAD_CPP_OPERATORS(pthread_eventflag_t,id)
} pthread_eventflag_t;


typedef struct pthread_eventflagattr_t
{
  int attr;
  int init;
} pthread_eventflagattr_t;


typedef struct pthread_once_t 
{
  int done;
  pthread_mutex_t in_progress;
} pthread_once_t;

#define PTHREAD_ONCE_INIT_                      { 0, PTHREAD_MUTEX_INITIALIZER }

#define PTHREAD_INIT()          do { pthread_init_(); } while(0)
#define CHECK_PT_PTR(ptr)       do { if (ptr == NULL) return EINVAL; } while(0)


EXTERN int pthread_mbx_destroy_np(pthread_mbx_t *mbx);
EXTERN int pthread_mbx_init_np(pthread_mbx_t *restrict mbx, 
                               const pthread_mbxattr_t *restrict attr);

EXTERN int pthread_mbx_send_np(pthread_mbx_t *mbx, 
                               pthread_mbxmsg_t *msg);

EXTERN int pthread_mbx_receive_np(pthread_mbx_t *mbx,
                                  pthread_mbxmsg_t **msg);

EXTERN int pthread_mbx_timedreceive_np(pthread_mbx_t *mbx,
                                       pthread_mbxmsg_t **msg,
                                       const struct timespec *abstime);

EXTERN int pthread_mbxattr_init_np(pthread_mbxattr_t *attr);
EXTERN int pthread_mbxattr_destroy_np(pthread_mbxattr_t *attr);

/** @} */


/* *************************************** */
/* ********** Event Flags (_np) ********** */
/* *************************************** */

/** @defgroup EventFlags Event Flags
 *
 * @{
 */

/*
 * The Event Flags mechanism is used to place a thread in a WAIT
 * state until either all the events of one of the events specified
 * by a bit in the pattern is set.
 *
 * The pthread_eventflag* functions returns similar return code than 
 * the corresponding standard functions in pthread.
 */

EXTERN int pthread_eventflag_destroy_np(pthread_eventflag_t *evtflag);
EXTERN int pthread_eventflag_init_np(pthread_eventflag_t *restrict evtflag,
                                     const pthread_eventflagattr_t *restrict attr);

EXTERN int pthread_eventflag_set_np(pthread_eventflag_t *evtflag, int pattern);

/* Definition of Wait/Standby mode of an event flag */

#define PSS_THREAD_EVENT_WAIT_MODE_OR			(0x00000001U)	
#define PSS_THREAD_EVENT_WAIT_MODE_AND			(0x00000002U)	
#define PSS_THREAD_EVENT_WAIT_MODE_CLEAR_ALL	(0x00000002U)	
#define PSS_THREAD_EVENT_WAIT_MODE_CLEAR_PAT	(0x00000004U)	


enum {
  PTHREAD_EVENTFLAG_WAITMODE_AND = PSS_THREAD_EVENT_WAIT_MODE_AND,
  PTHREAD_EVENTFLAG_WAITMODE_OR = PSS_THREAD_EVENT_WAIT_MODE_OR,
};

enum {
  PTHREAD_EVENTFLAG_CLEARMODE_ALL = PSS_THREAD_EVENT_WAIT_MODE_CLEAR_ALL,
  PTHREAD_EVENTFLAG_CLEARMODE_PAT = PSS_THREAD_EVENT_WAIT_MODE_CLEAR_PAT,
};

EXTERN int pthread_eventflag_wait_np(pthread_eventflag_t *evtflag, 
                                     unsigned int pattern,
                                     int waitmode,
                                     int clearmode,
                                     unsigned int *result);

EXTERN int pthread_eventflag_try_np(pthread_eventflag_t *evtflag, 
                                    unsigned int pattern,
                                    int waitmode,
                                    int clearmode,
                                    unsigned int *result);

EXTERN int pthread_eventflag_timedwait_np(pthread_eventflag_t *evtflag, 
                                          unsigned int pattern,
                                          int waitmode,
                                          int clearmode,
                                          const struct timespec *abstime,
                                          unsigned int *result);

EXTERN int pthread_eventflagattr_init_np(pthread_eventflagattr_t *attr);
EXTERN int pthread_eventflagattr_destroy_np(pthread_eventflagattr_t *attr);

/** @} */


/* **************************************** */
/* ********** Message Pipe (_np) ********** */
/* **************************************** */

/** @defgroup Pipe Message Pipe
 *
 * @{
 */

/*
 * The message pipe mechanism is similar to the mailbox mechanism
 * except that the data is passed by copy through a pipe.
 */

EXTERN int pthread_msgpipe_destroy_np(pthread_msgpipe_t *msgpipe);
EXTERN int pthread_msgpipe_init_np(pthread_msgpipe_t *restrict msgpipe,
                                   const pthread_msgpipeattr_t *restrict attr);

enum {
  PTHREAD_MSGPIPE_WAITMODE_FULL = 0x00000001,
  PTHREAD_MSGPIPE_WAITMODE_ASAP = 0x00000000,
};

EXTERN int pthread_msgpipe_send_np(pthread_msgpipe_t *msgpipe, 
                                   void *buffer,
                                   int size,
                                   int waitmode,
                                   int *result);

EXTERN int pthread_msgpipe_trysend_np(pthread_msgpipe_t *msgpipe, 
                                      void *buffer,
                                      int size,
                                      int waitmode,
                                      int *result);

EXTERN int pthread_msgpipe_timedsend_np(pthread_msgpipe_t *msgpipe, 
                                        void *buffer,
                                        int size,
                                        int waitmode,
                                        const struct timespec *abstime,
                                        int *result);

EXTERN int pthread_msgpipe_receive_np(pthread_msgpipe_t *msgpipe, 
                                      void *buffer,
                                      int size,
                                      int waitmode,
                                      int *result);

EXTERN int pthread_msgpipe_tryreceive_np(pthread_msgpipe_t *msgpipe, 
                                         void *buffer,
                                         int size,
                                         int waitmode,
                                         int *result);

EXTERN int pthread_msgpipe_timedreceive_np(pthread_msgpipe_t *msgpipe, 
                                           void *buffer,
                                           int size,
                                           int waitmode,
                                           const struct timespec *abstime,
                                           int *result);

EXTERN int pthread_msgpipeattr_init_np(pthread_msgpipeattr_t *attr);
EXTERN int pthread_msgpipeattr_destroy_np(pthread_msgpipeattr_t *attr);

/** @} */


/* ********************************************************* */
/* ********** Other Non-Portable functions (*_np) ********** */
/* ********************************************************* */

/** @defgroup Others Other functions
 *
 * @{
 */

/**
 * Return the number of processors for this hardware.
 * Always 1 on the PSP
 */

EXTERN int pthread_num_processors_np();


/** 
 * Put the current thread in WAIT state for the specified
 * amount of time
 */

EXTERN int pthread_sleep_np(int usecond);


/** 
 * Put the current thread in WAIT state for the specified
 * amount of time
 */

EXTERN int pthread_delay_np(const struct timespec *time);


/** 
 * Put the current thread in WAIT state for the specified
 * amount of time
 */

EXTERN int pthread_delay_until_np(const struct timespec *abstime);


/** 
 * Get the thread ID of the given thread
 * Return 0 (success) or EINVAL
 */

EXTERN int pthread_getthreadid_np(pthread_t thread, int *id);


/**
 * Gets elapsed time from system operation start.
 * Always return 0 (success)
 */

EXTERN int pthread_getsystemtime_np(struct timespec *t);


/* 
 * Does nothing for now.  Returns 0
 */

EXTERN void *pthread_timechange_handler_np (void *arg);

/** @} */

/*
 * Vita specific non-posix pthread extensions
 */
EXTERN int pthread_vita_tls_create_np (void (*func)(void*));
EXTERN void pthread_vita_tls_free_np (int key);
EXTERN int pthread_vita_tls_set_np (int key, void *value);
EXTERN void* pthread_vita_tls_get_np (int key);

#endif /* _H_pthread_vita */
