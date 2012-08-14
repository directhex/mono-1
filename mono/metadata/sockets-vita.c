/*
 * sockets-vita.c: Socket support for Vita
 *
 * This file contains the Vita implementation of functions in io-layer/sockets.c, plus a few
 * POSIX functions needed by socket-io.c.
 *
 * Based on:
 *
 * sockets.c:  Socket handles
 *
 * (C) 2002 Ximian, Inc.
 * (c) 2012 SCEA, LLC.
 */

#include <config.h>


#include "bridge.h"
#include "sockets-vita.h"
#include "pthread.h"

#include <mono/metadata/gc-internal.h>
#include <mono/io-layer/wapi.h>
#include <mono/io-layer/wapi-private.h>
#include <mono/io-layer/socket-private.h>
#include <mono/io-layer/handles-private.h>
#include <mono/io-layer/socket-wrappers.h>
#include <mono/utils/mono-membar.h>
#include <mono/utils/mono-poll.h>

#if 0
#define DEBUG(...) g_message(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

typedef struct {
	int fd;
} SocketData;

static mono_mutex_t socket_mutex = MONO_MUTEX_INITIALIZER;
static GHashTable *fd_to_handle;
static GHashTable *handle_to_data;

static inline void
sockets_lock (void)
{
	int res;

	res = mono_mutex_lock (&socket_mutex);
	g_assert (res == 0);
}

static inline void
sockets_unlock (void)
{
	int res;

	res = mono_mutex_unlock (&socket_mutex);
	g_assert (res == 0);
}


int
pss_error_to_wsa (int error)
{
	switch (error) {
	case PSS_NET_EPERM:
		return WSAEACCES;
	case PSS_NET_ENOENT:
		return WSAECONNREFUSED;
	/*
	  case PSS_NET_ESRCH:
	*/
	case PSS_NET_EINTR:
		return WSAEINTR;
	/*
case PSS_NET_EIO:
case PSS_NET_ENXIO:
case PSS_NET_E2BIG:
case PSS_NET_ENOEXEC:
case PSS_NET_EBADF:
case PSS_NET_ECHILD:
case PSS_NET_EDEADLK:
case PSS_NET_ENOMEM:
case PSS_NET_EACCES:
case PSS_NET_EFAULT:
case PSS_NET_ENOTBLK:
case PSS_NET_EBUSY:
case PSS_NET_EEXIST:
case PSS_NET_EXDEV:
case PSS_NET_ENODEV:
case PSS_NET_ENOTDIR:
case PSS_NET_EISDIR:
	*/
	case PSS_NET_EINVAL:
		return WSAEINVAL;
		/*
case PSS_NET_ENFILE:
case PSS_NET_EMFILE:
case PSS_NET_ENOTTY:
case PSS_NET_ETXTBSY:
case PSS_NET_EFBIG:
case PSS_NET_ENOSPC:
case PSS_NET_ESPIPE:
case PSS_NET_EROFS:
case PSS_NET_EMLINK:
	*/
	case PSS_NET_EPIPE:
		return WSAESHUTDOWN;
		/*
case PSS_NET_EDOM:
case PSS_NET_ERANGE:
		*/
case PSS_NET_EAGAIN:
	return WSAEWOULDBLOCK;
	/*
	//case PSS_NET_EWOULDBLOCK:
case PSS_NET_EINPROGRESS:
case PSS_NET_EALREADY:
case PSS_NET_ENOTSOCK:
case PSS_NET_EDESTADDRREQ:
case PSS_NET_EMSGSIZE:
case PSS_NET_EPROTOTYPE:
case PSS_NET_ENOPROTOOPT:
case PSS_NET_EPROTONOSUPPORT:
case PSS_NET_ESOCKTNOSUPPORT:
case PSS_NET_EOPNOTSUPP:
case PSS_NET_EPFNOSUPPORT:
case PSS_NET_EAFNOSUPPORT:
		*/
case PSS_NET_EADDRINUSE:
	return WSAEADDRINUSE;
	/*
case PSS_NET_EADDRNOTAVAIL:
case PSS_NET_ENETDOWN:
case PSS_NET_ENETUNREACH:
case PSS_NET_ENETRESET:
case PSS_NET_ECONNABORTED:
case PSS_NET_ECONNRESET:
case PSS_NET_ENOBUFS:
case PSS_NET_EISCONN:
case PSS_NET_ENOTCONN:
case PSS_NET_ESHUTDOWN:
case PSS_NET_ETOOMANYREFS:
	*/
	case PSS_NET_ETIMEDOUT:
	return WSAETIMEDOUT;
case PSS_NET_ECONNREFUSED:
	return WSAECONNREFUSED;
	/*
case PSS_NET_ELOOP:
case PSS_NET_ENAMETOOLONG:
case PSS_NET_EHOSTDOWN:
case PSS_NET_EHOSTUNREACH:
case PSS_NET_ENOTEMPTY:
case PSS_NET_EPROCLIM:
case PSS_NET_EUSERS:
case PSS_NET_EDQUOT:
case PSS_NET_ESTALE:
case PSS_NET_EREMOTE:
case PSS_NET_EBADRPC:
case PSS_NET_ERPCMISMATCH:
case PSS_NET_EPROGUNAVAIL:
case PSS_NET_EPROGMISMATCH:
case PSS_NET_EPROCUNAVAIL:
case PSS_NET_ENOLCK:
case PSS_NET_ENOSYS:
case PSS_NET_EFTYPE:
case PSS_NET_EAUTH:
case PSS_NET_ENEEDAUTH:
case PSS_NET_EIDRM:
case PSS_NET_ENOMSG:
case PSS_NET_EOVERFLOW:
case PSS_NET_EILSEQ:
case PSS_NET_ENOTSUP:
case PSS_NET_ECANCELED:
case PSS_NET_EBADMSG:
case PSS_NET_ENODATA:
case PSS_NET_ENOSR:
case PSS_NET_ENOSTR:
case PSS_NET_ETIME:
case PSS_NET_EADHOC:
case PSS_NET_EDISABLEDIF:
case PSS_NET_ERESUME:
case PSS_NET_EIPADDRCHANGED:
	*/
	default:
		DEBUG ("pss_error_to_wsa () unable to map: %d.", error);
		return WSAEFAULT;
	}
}

static inline void
wsa_set_error_from_errno (int err)
{
	WSASetLastError (pss_error_to_wsa (err));
}

static struct _WapiHandle_socket*
get_socket_handle_data (guint32 fd)
{
	gboolean ok;
	gpointer handle;
	struct _WapiHandle_socket *res;

	sockets_lock ();
	handle = g_hash_table_lookup (fd_to_handle, (gconstpointer) fd);
	sockets_unlock ();
	g_assert (handle);

	ok = _wapi_lookup_handle (handle, WAPI_HANDLE_SOCKET,
							  (gpointer *)&res);
	if (!ok) {
			g_warning ("%s: error looking up socket handle %p",
				   __func__, handle);
		return NULL;
	} else {
		return res;
	}
}

static gpointer
create_socket (int fd, int domain, int type, int protocol)
{
	struct _WapiHandle_socket socket_handle = {0};
	gpointer handle;
	SocketData *data;

	data = g_new0 (SocketData, 1);
	
	socket_handle.domain = domain;
	socket_handle.type = type;
	socket_handle.protocol = protocol;
	socket_handle.still_readable = 1;

	data->fd = fd;

	/*
	 * On the PSP2, socket and file descriptors can overlap, so we can't use
	 * _wapi_handle_new_fd. Instead, we use a normal handle, and save the association in
	 * a hash table.
	 */
	handle = _wapi_handle_new (WAPI_HANDLE_SOCKET, &socket_handle);
	if (handle == _WAPI_HANDLE_INVALID)
		return _WAPI_HANDLE_INVALID;

	sockets_lock ();
	g_hash_table_insert (fd_to_handle, (gpointer) fd, handle);
	g_hash_table_insert (handle_to_data, handle, data);
	sockets_unlock ();

	return handle;
}



static void socket_close (gpointer handle, gpointer data);

struct _WapiHandleOps _wapi_socket_ops = {
	socket_close,		/* close */
	NULL,			/* signal */
	NULL,			/* own */
	NULL,			/* is_owned */
	NULL,			/* special_wait */
	NULL			/* prewait */
};

int
WSAStartup (guint32 requested, WapiWSAData *data)
{
	// This can be called from debugger-agent.c too
	sockets_lock ();

	pss_net_init ();

	fd_to_handle = g_hash_table_new (NULL, NULL);
	handle_to_data = g_hash_table_new (NULL, NULL);

	sockets_unlock ();

	return 0;
}

int
WSACleanup (void)
{
	return 0;
}

void
WSASetLastError (int error)
{
	DEBUG ("WSASetLastError (): %d.\n", error);
	SetLastError (error);
}

int
WSAGetLastError (void)
{
	return(GetLastError ());
}

static void
socket_close (gpointer handle, gpointer data)
{
	SocketData *sdata;

	sockets_lock ();
	sdata = g_hash_table_lookup (handle_to_data, handle);
	sockets_unlock ();
	g_assert (sdata);

	DEBUG ("socket_close (): %d %d", handle, sdata->fd);

	mono_gc_set_skip_thread (TRUE);
	pss_net_shutdown (sdata->fd, SHUT_RD);
	pss_net_socket_close (sdata->fd);
	mono_gc_set_skip_thread (FALSE);
}

int
closesocket (guint32 fd)
{
	gpointer handle;

	if (fd == 0)
		return (0);

	sockets_lock ();
	handle = g_hash_table_lookup (fd_to_handle, (gconstpointer) fd);
	sockets_unlock ();
	g_assert (handle);

	DEBUG ("closesocket (): %d %d", fd, handle);

	if (_wapi_handle_type (handle) != WAPI_HANDLE_SOCKET) {
		WSASetLastError (WSAENOTSOCK);
		return(0);
	}
	
	_wapi_handle_unref (handle);

	return(0);
}

guint32
_wapi_accept (guint32 fd, struct sockaddr *addr, socklen_t *addrlen)
{
	int res;
	gpointer handle;
	struct _WapiHandle_socket *socket_handle;

	DEBUG ("accept (): %d", fd);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_accept (fd, addr, addrlen);
	mono_gc_set_skip_thread (FALSE);

	if (res < 0) {
		DEBUG ("accept () failed with: %d.", pss_net_errno);
		wsa_set_error_from_errno (pss_net_errno);
		return INVALID_SOCKET;
	}

	DEBUG ("accept returned (): %d", res);

	socket_handle = get_socket_handle_data (fd);

	handle = create_socket (res, socket_handle->domain, socket_handle->type, socket_handle->protocol);
	if (handle == _WAPI_HANDLE_INVALID)
		return _WAPI_HANDLE_INVALID;

	return res;
}

int
_wapi_bind (guint32 fd, struct sockaddr *my_addr, socklen_t addrlen)
{
	int res;

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_bind (fd, my_addr, addrlen);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0) {
		DEBUG ("bind () failed with: %d.\n", pss_net_errno);
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}

	return 0;
}

int
_wapi_connect (guint32 fd, const struct sockaddr *serv_addr,
			   socklen_t addrlen)
{
	int res;

	DEBUG ("connect (): %d %d", fd, addrlen);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_connect (fd, serv_addr, addrlen);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0) {
		DEBUG ("connect () failed with: %d.\n", pss_net_errno);
		if (pss_net_errno == PSS_NET_EINPROGRESS)
			wsa_set_error_from_errno (PSS_NET_EWOULDBLOCK);
		else
			wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}

	return 0;
}
	
int
_wapi_getpeername (guint32 fd, struct sockaddr *name, socklen_t *namelen)
{
	int res;

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_getpeername (fd, name, namelen);
	mono_gc_set_skip_thread (FALSE);

	if (res == 0) {
		return 0;
	} else {
		wsa_set_error_from_errno (pss_net_errno);
		return -1;
	}
}

int
_wapi_getsockname (guint32 fd, struct sockaddr *name, socklen_t *namelen)
{
	int res;

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_getsockname (fd, name, namelen);
	mono_gc_set_skip_thread (FALSE);

	if (res == 0) {
		return 0;
	} else {
		wsa_set_error_from_errno (pss_net_errno);
		return -1;
	}
}

int
_wapi_getsockopt (guint32 fd, int level, int optname, void *optval,
				  socklen_t *optlen)
{
	int res;

	DEBUG ("getsockopt (): %d %d %d", fd, level, optname);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_getsockopt (fd, level, optname, optval, optlen);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}
	return 0;
}

int
_wapi_setsockopt (guint32 fd, int level, int optname,
				  const void *optval, socklen_t optlen)
{
	int res;

	DEBUG ("_wapi_setsockopt (): %d %d %d", fd, level, optname);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_setsockopt (fd, level, optname, optval, optlen);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}
	return 0;
}

int
_wapi_listen (guint32 fd, int backlog)
{
	int res;

	/* pss_net_listen () doesn't seem to support -1 */
	if (backlog == -1)
		backlog = 5;

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_listen (fd, backlog);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0) {
		DEBUG ("listen () failed with: %d.\n", pss_net_errno);
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}

	return 0;
}

int
_wapi_recv (guint32 fd, void *buf, size_t len, int recv_flags)
{
	return _wapi_recvfrom (fd, buf, len, recv_flags, NULL, NULL);
}

int _wapi_recvfrom (guint32 fd, void *buf, size_t len, int recv_flags,
					struct sockaddr *from, socklen_t *fromlen)
{
	int res;

	DEBUG ("recvfrom (): %d %d.", fd, len);

	mono_gc_set_skip_thread (TRUE);
	if (from == NULL)
		res = pss_net_recv (fd, buf, len, recv_flags);
	else
		res = pss_net_recvfrom (fd, buf, len, recv_flags, from, fromlen);
	mono_gc_set_skip_thread (FALSE);

	DEBUG ("recvfrom returned (): %d %d.", fd, res);

	if (res < 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}

	return res;
}

int _wapi_send (guint32 fd, const void *msg, size_t len, int send_flags)
{
	int res;

	DEBUG ("send (): %d %d", fd, len);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_send (fd, msg, len, 0);
	mono_gc_set_skip_thread (FALSE);

	DEBUG ("send returned (): %d %x", fd, res);
	if (res < 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}

	return res;
}

int
_wapi_sendto (guint32 fd, const void *msg, size_t len, int send_flags,
			  const struct sockaddr *to, socklen_t tolen)
{
	int res;

	DEBUG ("sendto (): %d %d", fd, len);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_sendto (fd, msg, len, 0, to, tolen);
	mono_gc_set_skip_thread (FALSE);

	DEBUG ("sendto returned (): %d %x", fd, res);
	if (res < 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return SOCKET_ERROR;
	}

	return res;
}

int
_wapi_shutdown (guint32 fd, int how)
{
	struct _WapiHandle_socket *socket_handle;
	int res;

	DEBUG ("shutdown (): %d %d", fd, how);

	/* See _wapi_shutdown () in sockets.c */
	if (how == SHUT_RD || how == SHUT_RDWR) {
		socket_handle = get_socket_handle_data (fd);
		if (!socket_handle) {
			WSASetLastError (WSAENOTSOCK);
			return(SOCKET_ERROR);
		}
		
		socket_handle->still_readable = 0;
	}

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_shutdown (fd, how);
	mono_gc_set_skip_thread (FALSE);

	/* If the network layer has forcibly shutdown the socket
	 * we might get an error here, let's not expose that */
	if (res == -2143223530/* SCE_NET_ERROR_EINVAL */ )
		res = 0;

	DEBUG ("shutdown returned (): %d %d", fd, res);

	if (res < 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return -1;
	} else {
		return 0;
	}
}

guint32 _wapi_socket (int domain, int type, int protocol, void *unused,
					  guint32 unused2, guint32 unused3)
{
	gpointer handle;
	int fd;

	mono_gc_set_skip_thread (TRUE);
	fd = pss_net_socket ("", domain, type, protocol);
	mono_gc_set_skip_thread (FALSE);

	if (fd < 0) {
		wsa_set_error_from_errno (pss_net_errno);
		return(INVALID_SOCKET);
	}

	handle = create_socket (fd, domain, type, protocol);
	if (handle == _WAPI_HANDLE_INVALID)
		return INVALID_SOCKET;

	DEBUG ("socket (): %d %d", fd, handle);

	return fd;
}

static int
get_resolver (void)
{
	static int resolver_id = -1;
	int id;

	if (resolver_id == -1) {
		sockets_lock ();

		if (resolver_id == -1) {
			mono_gc_set_skip_thread (TRUE);
			id = pss_net_resolver_create ("", NULL, 0);
			mono_gc_set_skip_thread (FALSE);
			g_assert (id >= 0);
			mono_memory_barrier ();
			resolver_id = id;
		}
		sockets_unlock ();
	}

	return resolver_id;
}

/*
 * Same as gethostbyname, but the result is dynamically allocated, and needs to be freed
 * using _wapi_freehostent ().
 */
PssNetHostent *
_wapi_gethostbyname_vita (const char *hostname)
{
	int resolver_id, res;
	PssNetInAddr addr;
	uint32_t *paddr;
	PssNetHostent *he;

	DEBUG ("gethostbyname (): '%s'", hostname);

	resolver_id = get_resolver ();

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_resolver_start_ntoa (resolver_id, hostname, &addr, 0, 0, 0);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0)
		return NULL;

	he = g_malloc0 (sizeof (PssNetHostent));
	he->h_name = g_strdup (hostname);
	he->h_addrtype = AF_INET;
	he->h_length = 4;
	he->h_aliases = g_malloc0 (sizeof (gpointer));
	he->h_addr_list = g_malloc0 (sizeof (gpointer) * 2);
	paddr = g_malloc0 (sizeof (uint32_t));
	memcpy (paddr, &addr.s_addr, sizeof (uint32_t));
	he->h_addr_list [0] = (char*)paddr;

	DEBUG ("gethostbyname () returned.\n");

	return he;
}

void
_wapi_freehostent (PssNetHostent *he)
{
	if (!he)
		return;

	g_free (he->h_name);
	g_free (he->h_aliases);
	g_free (he->h_addr_list [0]);
	g_free (he->h_addr_list);
	g_free (he);
}

gboolean
TransmitFile (guint32 socket, gpointer file, guint32 bytes_to_write, guint32 bytes_per_send, WapiOverlapped *ol,
		WapiTransmitFileBuffers *buffers, guint32 flags)
{
	return FALSE;
}

int
WSAIoctl (guint32 fd, gint32 command,
	  gchar *input, gint i_len,
	  gchar *output, gint o_len, glong *written,
	  void *unused1, void *unused2)
{
	WSASetLastError (WSAEINVAL);
	return SOCKET_ERROR;
}

int
WSARecv (guint32 fd, WapiWSABuf *buffers, guint32 count, guint32 *received,
	     guint32 *flags, WapiOverlapped *overlapped,
	     WapiOverlappedCB *complete)
{
	WSASetLastError (WSAEINVAL);
	return SOCKET_ERROR;
}

int
WSASend (guint32 fd, WapiWSABuf *buffers, guint32 count, guint32 *sent,
		 guint32 flags, WapiOverlapped *overlapped,
		 WapiOverlappedCB *complete)
{
	WSASetLastError (WSAEINVAL);
	return SOCKET_ERROR;
}

/*
 * POSIX functions required by socket-io.c.
 */
int inet_aton (const char *cp, struct in_addr *inp)
{
	int resolver_id, res;

	DEBUG ("inet_aton (): %s", cp);

	resolver_id = get_resolver ();
	g_assert (resolver_id >= 0);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_resolver_start_ntoa (resolver_id, cp, inp, 0, 0, 0);
	mono_gc_set_skip_thread (FALSE);

	if (res == 0) {
		DEBUG ("inet_aton () returned : 1");
		return 1;
	} else {
		DEBUG ("inet_aton () returned : 0");
		return 0;
	}
}

/*
 * Same as gethostbyname, but the result is dynamically allocated, and needs to be freed
 * using _wapi_freehostent ().
 */
PssNetHostent*
gethostbyaddr_vita (const char *addr,
					int len,
					int type)
{
	int resolver_id, res;
	PssNetInAddr *iaddr = (void*)addr;
	uint32_t *paddr;
	PssNetHostent *he;
	char *hostname;

	g_assert (type == AF_INET);

	DEBUG ("gethostbyaddr ():");

	resolver_id = get_resolver ();
	g_assert (resolver_id >= 0);

	hostname = g_malloc (1024);

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_resolver_start_aton (resolver_id, iaddr, hostname, 1024, 0, 0, 0);
	mono_gc_set_skip_thread (FALSE);

	if (res != 0) {
		DEBUG ("gethostbyaddr () failed: %x", res);
		return NULL;
	}

	he = g_malloc0 (sizeof (PssNetHostent));
	he->h_name = g_strdup (hostname);
	he->h_addrtype = AF_INET;
	he->h_length = 4;
	he->h_aliases = g_malloc0 (sizeof (gpointer));
	he->h_addr_list = g_malloc0 (sizeof (gpointer) * 2);
	paddr = g_malloc0 (sizeof (uint32_t));
	memcpy (paddr, &iaddr->s_addr, sizeof (uint32_t));
	he->h_addr_list [0] = (char*)paddr;

	g_free (hostname);

	DEBUG ("gethostbyaddr () returned: %s.", he->h_name);

	return he;
}

int
gethostname (char *name, size_t namelen)
{
	int res;

	mono_gc_set_skip_thread (TRUE);
	res = pss_net_gethostname (name, namelen);
	mono_gc_set_skip_thread (FALSE);

	return res;
}
