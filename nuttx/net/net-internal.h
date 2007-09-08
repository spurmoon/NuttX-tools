/****************************************************************************
 * net-internal.h
 *
 *   Copyright (C) 2007 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name Gregory Nutt nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __NET_INTERNAL_H
#define __NET_INTERNAL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#ifdef CONFIG_NET

#include <nuttx/net.h>

#include "net-internal.h"

/****************************************************************************
 * Definitions
 ****************************************************************************/

/* This macro converts a socket option value into a bit setting */

#define _SO_BIT(o)       (1 << (o))

/* These define bit positions for each socket option (see sys/socket.h) */

#define _SO_DEBUG        _SO_BIT(SO_DEBUG)
#define _SO_ACCEPTCONN   _SO_BIT(SO_ACCEPTCONN)
#define _SO_BROADCAST    _SO_BIT(SO_BROADCAST)
#define _SO_REUSEADDR    _SO_BIT(SO_REUSEADDR)
#define _SO_KEEPALIVE    _SO_BIT(SO_KEEPALIVE)
#define _SO_LINGER       _SO_BIT(SO_LINGER)
#define _SO_OOBINLINE    _SO_BIT(SO_OOBINLINE)
#define _SO_SNDBUF       _SO_BIT(SO_SNDBUF)
#define _SO_RCVBUF       _SO_BIT(SO_RCVBUF)
#define _SO_ERROR        _SO_BIT(SO_ERROR)
#define _SO_TYPE         _SO_BIT(SO_TYPE)
#define _SO_DONTROUTE    _SO_BIT(SO_DONTROUTE)
#define _SO_RCVLOWAT     _SO_BIT(SO_RCVLOWAT)
#define _SO_RCVTIMEO     _SO_BIT(SO_RCVTIMEO)
#define _SO_SNDLOWAT     _SO_BIT(SO_SNDLOWAT)
#define _SO_SNDTIMEO     _SO_BIT(SO_SNDTIMEO)

/* This is the larget option value */

#define _SO_MAXOPT       (15)

/* Macros to set, test, clear options */

#define _SO_SETOPT(s,o)  ((s) |= _SO_BIT(o))
#define _SO_CLROPT(s,o)  ((s) &= ~_SO_BIT(o))
#define _SO_GETOPT(s,o)  (((s) & _SO_BIT(o)) != 0)

/* These are macros that can be used to determine if socket option code is
 * valid (in range) and supported by API.
 */

#define _SO_GETONLYSET   (_SO_ACCEPTCONN|_SO_ERROR|_SO_TYPE)
#define _SO_GETONLY(o)   ((_SO_BIT(o) & _SO_GETONLYSET) != 0)
#define _SO_GETVALID(o)  (((unsigned int)(o)) <= _SO_MAXOPT)
#define _SO_SETVALID(o)  ((((unsigned int)(o)) <= _SO_MAXOPT) && !_SO_GETONLY(o))

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Variables
 ****************************************************************************/

/****************************************************************************
 * Pulblic Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/* net-sockets.c *************************************************************/

EXTERN void weak_function net_initialize(void);
EXTERN int  sockfd_allocate(void);
EXTERN void sockfd_release(int sockfd);
EXTERN FAR struct socket *sockfd_socket(int sockfd);

/* sockopt support ***********************************************************/

#if defined(CONFIG_NET_SOCKOPTS) && !defined(CONFIG_DISABLE_CLOCK)
EXTERN int net_timeo(uint32 start_time, socktimeo_t timeo);
EXTERN socktimeo_t socktimeo_t net_timeval2dsec(struct timeval *tv);
EXTERN void net_dsec2timeval(uint16 dsec, struct timeval *tv);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* CONFIG_NET */
#endif /* __NET_INTERNAL_H */