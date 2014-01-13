/*
 * Copyright (c) 2000-2007 Niels Provos <provos@citi.umich.edu>
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _LOG_H_
#define _LOG_H_

#include "event2/util.h"

#ifdef __GNUC__
#define EV_CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
#define EV_NORETURN __attribute__((noreturn))
#else
#define EV_CHECK_FMT(a,b)
#define EV_NORETURN
#endif

#define _EVENT_ERR_ABORT ((int)0xdeaddead)

void event_err_(const char* file, int line,int eval, const char *fmt, ...) EV_CHECK_FMT(4,5) EV_NORETURN;
void event_warn_(const char* file, int line,const char *fmt, ...) EV_CHECK_FMT(3,4);
void event_sock_err_(const char* file, int line,int eval, evutil_socket_t sock, const char *fmt, ...) EV_CHECK_FMT(5,6) EV_NORETURN;
void event_sock_warn_(const char* file, int line,evutil_socket_t sock, const char *fmt, ...) EV_CHECK_FMT(4,5);
void event_errx_(const char* file, int line,int eval, const char *fmt, ...) EV_CHECK_FMT(4,5) EV_NORETURN;
void event_warnx_(const char* file, int line,const char *fmt, ...) EV_CHECK_FMT(3,4);
void event_msgx_(const char* file, int line,const char *fmt, ...) EV_CHECK_FMT(3,4);
void _event_debugx(const char* file, int line,const char *fmt, ...) EV_CHECK_FMT(3,4);


#define event_err(...) event_err_(__FILE__,__LINE__,__VA_ARGS__)
#define event_warn(...) event_warn_(__FILE__,__LINE__,__VA_ARGS__)
#define event_sock_err(...) event_sock_err_(__FILE__,__LINE__,__VA_ARGS__)
#define event_sock_warn(...) event_sock_warn_(__FILE__,__LINE__,__VA_ARGS__)
#define event_errx(...) event_errx_(__FILE__,__LINE__,__VA_ARGS__)
#define event_warnx(...) event_warnx_(__FILE__,__LINE__,__VA_ARGS__)
#define event_msgx(...) event_msgx_(__FILE__,__LINE__,__VA_ARGS__)

#ifdef USE_DEBUG
#define event_debug(...) _event_debugx(__FILE__,__LINE__,__VA_ARGS__)
#else
#define event_debug(...)
#endif

#undef EV_CHECK_FMT

#endif
