/*
 * Network Performance Meter
 * Copyright (C) 2016-2017 by Thomas Dreibholz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: dreibh@iem.uni-due.de
 */

#ifndef TOOLS_H
#define TOOLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ext_socket.h>

#include <iostream>


/* MPTCP as "pseudo-protocol". Just for internal representation. */
#define IPPROTO_MPTCP IPPROTO_EGP

/* FIXME: This is ugly, but currently the only way to easily get the #defines for Linux MPTCP! */
#define TCP_MULTIPATH_DEBUG         10001   /* MPTCP DEBUG on/off  */
#define TCP_MULTIPATH_ENABLE        42      /* MPTCP ENABLE on/off */
#define TCP_MULTIPATH_ENABLE_LEGACY 10002
#define TCP_MULTIPATH_ADD           10003
#define TCP_MULTIPATH_REMOVE        10004
#define TCP_MULTIPATH_SUBFLOWS      10005
#define TCP_MULTIPATH_CONNID        10006
#define TCP_MULTIPATH_NDIFFPORTS    10007   /* MPTCP NDIFFPORTS  */
#define TCP_MULTIPATH_PATHMANAGER   10008   /* MPTCP PATHMANAGER */
#define TCP_MULTIPATH_SCHEDULER     10009   /* MPTCP SCHEDULER   */


/* DCCP definitions */
#ifdef HAVE_DCCP
#include <linux/dccp.h>
#ifndef SOL_DCCP
#define SOL_DCCP 269
#endif
// #else
// #warning DCCP is not supported by the API of this system!
#endif


std::string format(const char* fmt, ...);

unsigned long long getMicroTime();
void printTimeStamp(std::ostream& os);
int pollTimeout(const unsigned long long now, const size_t n, ...);


int safestrcpy(char* dest, const char* src, const size_t size);
int safestrcat(char* dest, const char* src, const size_t size);

bool hasSuffix(const std::string& name, const std::string& suffix);
void dissectName(const std::string& name,
                 std::string&       prefix,
                 std::string&       suffix);


union sockaddr_union {
   struct sockaddr     sa;
   struct sockaddr_in  in;
   struct sockaddr_in6 in6;
};

bool checkIPv6();
size_t getSocklen(const struct sockaddr* address);
int addresscmp(const struct sockaddr* a1, const struct sockaddr* a2, const bool port);
bool address2string(const struct sockaddr* address,
                    char*                  buffer,
                    const size_t           length,
                    const bool             port,
                    const bool             hideScope = false);
bool string2address(const char*           string,
                    union sockaddr_union* address,
                    const bool            readPort = true);

void printAddress(std::ostream&          os,
                  const struct sockaddr* address,
                  const bool             port      = true,
                  const bool             hideScope = false);

const char* getProtocolName(const uint8_t protocol);
uint16_t getPort(const struct sockaddr* address);
bool setPort(struct sockaddr* address, uint16_t port);
bool sendAbort(int sd, sctp_assoc_t assocID = 0);
int createAndBindSocket(const int             family,
                        const int             type,
                        const int             protocol,
                        const uint16_t        localPort,
                        const unsigned int    localAddresses,
                        const sockaddr_union* localAddressArray,
                        const bool            listenMode,
                        const bool            bindV6Only);
uint64_t hton64(const uint64_t value);
uint64_t ntoh64(const uint64_t value);


typedef unsigned long long network_double_t;

network_double_t doubleToNetwork(double value);
double networkToDouble(const network_double_t value);


void installBreakDetector();
void uninstallBreakDetector();
bool breakDetected();
void sendBreak(const bool quiet);


#define RANDOM_CONSTANT    0
#define RANDOM_UNIFORM     1
#define RANDOM_EXPONENTIAL 2
#define RANDOM_PARETO      3

const char* getRandomGeneratorName(const uint8_t rng);
double getRandomValue(const double* valueArray, const uint8_t rng);
uint8_t random8();
uint16_t random16();
uint64_t random64();
uint32_t random32();
double randomDouble();
double randomExpDouble(const double p);
double randomParetoDouble(const double m, const double k);

#ifdef __APPLE__
// Apple's poll() function is broken. We need a wrapper to select() here!
int ext_poll_wrapper(struct pollfd* fdlist, long unsigned int count, int time);
#else
inline int ext_poll_wrapper(struct pollfd* fdlist, long unsigned int count, int time)
{
   return(ext_poll(fdlist, count, time));
}
#endif

#if defined(linux)
#warning Added fix for broken sctp_send() with LK-SCTP
ssize_t sctp_send_fixed(int                           sd,
                        const void*                   data,
                        size_t                        len,
                        const struct sctp_sndrcvinfo* sinfo,
                        int                           flags);
#define sctp_send(a,b,c,d,e) sctp_send_fixed(a,b,c,d,e)
#endif

bool setBufferSizes(int sd, const int sndBufSize, const int rcvBufSize);

#endif
