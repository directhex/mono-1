/*
 * Copyright 2012 SCEA, LLC
 */

#ifndef __EPOLL_VITA_H__
#define __EPOLL_VITA_H__

#define epoll_create(x) pss_net_epoll_create("ScePssMonoThreadPoolEpoll", 0)
#define epoll_wait pss_net_epoll_wait
#define epoll_ctl pss_net_epoll_ctl
#define epoll_destroy pss_net_epoll_destroy
#define epoll_event PssNetEpollEvent
#define EPOLLOUT PSS_NET_EPOLLOUT
#define EPOLLIN PSS_NET_EPOLLIN
#define EPOLL_CTL_ADD PSS_NET_EPOLL_CTL_ADD
#define EPOLL_CTL_MOD PSS_NET_EPOLL_CTL_MOD
#define EPOLL_CTL_DEL PSS_NET_EPOLL_CTL_DEL

#define EPOLLERR -1
#define EPOLLHUP -1

#undef EEXIST
#define EEXIST PSS_NET_EEXIST

#endif
