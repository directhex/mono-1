/*
 * (c) 2012 SCEA, LLC
*/

#ifndef __SOCKETS_VITA_H__
#define __SOCKETS_VITA_H__

#include "bridge.h"

#define AF_INET PSS_NET_AF_INET
#define SOCK_STREAM PSS_NET_SOCK_STREAM
#define SOCK_DGRAM PSS_NET_SOCK_DGRAM
#define SOCK_RAW PSS_NET_SOCK_RAW
#define SOL_SOCKET PSS_NET_SOL_SOCKET
#define SOL_IP PSS_NET_IPPROTO_IP
#define SOL_TCP PSS_NET_IPPROTO_TCP
#define SO_LINGER PSS_NET_SO_LINGER
#define SO_REUSEADDR PSS_NET_SO_REUSEADDR
#define SO_KEEPALIVE PSS_NET_SO_KEEPALIVE
#define SO_BROADCAST PSS_NET_SO_BROADCAST
#define SO_OOBINLINE PSS_NET_SO_OOBINLINE
#define SO_SNDBUF PSS_NET_SO_SNDBUF
#define SO_RCVBUF PSS_NET_SO_RCVBUF
#define SO_SNDLOWAT PSS_NET_SO_SNDLOWAT
#define SO_RCVLOWAT PSS_NET_SO_RCVLOWAT
#define SO_SNDTIMEO PSS_NET_SO_SNDTIMEO
#define SO_RCVTIMEO PSS_NET_SO_RCVTIMEO
#define SO_ERROR PSS_NET_SO_ERROR
#define SO_TYPE PSS_NET_SO_TYPE
#define IP_HDRINCL PSS_NET_IP_HDRINCL
#define IP_TOS PSS_NET_IP_TOS
#define IP_TTL PSS_NET_IP_TTL
#define IP_MULTICAST_IF PSS_NET_IP_MULTICAST_IF
#define IP_MULTICAST_TTL PSS_NET_IP_MULTICAST_TTL
#define IP_MULTICAST_LOOP PSS_NET_IP_MULTICAST_LOOP
#define IP_ADD_MEMBERSHIP PSS_NET_IP_ADD_MEMBERSHIP
#define IP_DROP_MEMBERSHIP PSS_NET_IP_DROP_MEMBERSHIP
#define TCP_NODELAY PSS_NET_TCP_NODELAY
#define SHUT_RD PSS_NET_SHUT_RD
#define SHUT_WR PSS_NET_SHUT_WR
#define SHUT_RDWR PSS_NET_SHUT_RDWR
#define POLLOUT PSS_NET_EPOLLOUT
#define INADDR_NONE 0xffffffff

#define hostent _PssNetHostent
#define sockaddr _PssNetSockaddr
#define sockaddr_in _PssNetSockaddrIn
#define in_addr _PssNetInAddr
#define linger _PssNetLinger
#define ip_mreq _PssNetIpMreq
#define ntohs pss_net_ntohs
#define ntohl pss_net_ntohl
#define htons pss_net_htons
#define htonl pss_net_htonl

PssNetHostent* _wapi_gethostbyname_vita (const char *hostname);
void _wapi_freehostent (PssNetHostent *he);
int inet_aton (const char *cp, struct in_addr *inp);
PssNetHostent* gethostbyaddr_vita (const char *addr, int len, int type);
int gethostname (char *name, size_t namelen);

#endif
