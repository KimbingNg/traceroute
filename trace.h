//
// Created by Kimbing Ng on 3/15/20.
//

#ifndef MYTRACEROUTE_TRACE_H
#define MYTRACEROUTE_TRACE_H

#include "unp.h"
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>


#define BUFSIZE 1500

struct rec {
    u_short        rec_seq;
    u_short        rec_ttl;
    struct timeval rec_tv;
};


extern char recvbuf[BUFSIZE];
extern char sendbuf[BUFSIZE];

extern int        datalen;
extern char *     host;
extern u_short    sport, dport;
extern int        nsent;
extern pid_t      pid;
extern  int prob, nprobes;
extern int        sendfd, recvfd;
extern int        ttl, max_ttl;
extern int        verbose;

const char *icmpcode_v4(int);
const char *icmpcode_v6(int);

int  recv_v4(int, struct timeval *);
int  recv_v6(int, struct timeval *);
void sig_alarm(int);
void traceloop(void);
void tv_sub(struct timeval *, struct timeval *);

struct proto {
    const char *(*icmpcode)(int);
    int (*recv)(int, struct timeval *);
    struct sockaddr *sasend;
    struct sockaddr *sarecv;
    struct sockaddr *salast;
    struct sockaddr *sabind;
    socklen_t        salen;
    int              icmpproto;
    int              ttllevel;
    int              ttloptname;
} ;

extern struct  proto * pr;

#ifdef IPV6
#    include <netinet/icmp6.h>
#    include <netinet/ip6.h>

#endif


#endif  // MYTRACEROUTE_TRACE_H
