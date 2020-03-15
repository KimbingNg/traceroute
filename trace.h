//
// Created by Kimbing Ng on 3/15/20.
//

#ifndef MYTRACEROUTE_TRACE_H
#define MYTRACEROUTE_TRACE_H

#include "traceutil.h"
#include "unp.h"
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#define TRACE_RESULT_TIMEOUT -3
#define TRACE_RESULT_TIMEEXCEED -2
#define TRACE_RESULT_UNREACH -1
#define ICMP_HLEN 8
#define UDP_HLEN 8


#define BUFSIZE 1500

struct rec {
    u_short        rec_seq;
    u_short        rec_ttl;
    struct timeval rec_tv;
};


extern char recvbuf[BUFSIZE];
extern char sendbuf[BUFSIZE];

extern int     datalen;
extern char *  host;
extern u_short sport, dport;
extern int     nsent;
extern pid_t   pid;
extern int     probe, nprobes;
extern int     sendfd, recvfd;
extern int     ttl, max_ttl;
extern int     verbose;


void sig_alarm(int);
void traceloop(void);
void tv_sub(struct timeval *, struct timeval *);

struct proto {
    const char *(*icmpcode)(int);
    int (*recv)(int, struct timeval *);  // receive function
    struct sockaddr *sendAddr;
    struct sockaddr *receiveAddr;
    struct sockaddr *lastAddr;
    struct sockaddr *bindAddr;
    socklen_t        addrLen;
    int              icmpproto;
    int              ttllevel;
    int              ttloptname;
};

extern struct proto *pr;

#ifdef IPV6
#    include <netinet/icmp6.h>
#    include <netinet/ip6.h>

#endif


#endif  // MYTRACEROUTE_TRACE_H
