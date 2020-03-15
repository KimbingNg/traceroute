#include "trace.h"
#include <iostream>
/*
struct proto proto_v4 = {.icmpcode   = icmpcode_v4,
                         .recv       = recv_v4,
                         .sasend     = NULL,
                         .sarecv     = NULL,
                         .salast     = NULL,
                         .sabind     = NULL,
                         .salen      = 0,
                         .icmpproto  = IPPROTO_ICMP,
                         .ttllevel   = IPPROTO_IP,
                         .ttloptname = IP_TTL};

 */
#ifdef IPV6
struct proto proto_v6 = {.icmpcode   = icmpcode_v6,
                         .recv       = recv_v6,
                         .sasend     = NULL,
                         .sarecv     = NULL,
                         .salast     = NULL,
                         .sabind     = NULL,
                         .salen      = 0,
                         .icmpproto  = IPPROTO_ICMP6,
                         .ttllevel   = IPPROTO_IP6,
                         .ttloptname = IPV6_UNICAST_HOPS};
#endif

int     datalen = sizeof(struct rec);
int     max_ttl = 30;
int     nprobes = 3;
u_short dport   = 32768 + 666;
int        verbose;
pid_t      pid;


char *host;

char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];


int main(int argc, char *argv[]) {
    int              c;
    struct addrinfo *ai;
    char *           h;
    opterr = 0;
    while ((c = getopt(argc, argv, "m:v")) != -1) {
        switch (c) {
            case 'm':
                if ((max_ttl = atoi(optarg)) <= 1)
                    err_quit("invalid -m value");
                break;
            case 'v': ++verbose; break;
            case '?':
            default: err_quit("unrecognized option: %c", c);
        }
    }

    if (optind != argc - 1)
        err_quit("usage: traceroute [-m <maxttl> -v] <hostname>");
    host = argv[optind];

    pid = getpid();
    signal(SIGALRM, sig_alarm);

    ai = host_serv(host, NULL, 0, 0);

    h = sock_ntop(ai->ai_addr,ai->ai_addrlen);
    
    printf("traceroute to %s(%s):%d hops max, %d data bytes\n", ai->ai_canonname?ai->ai_canonname:h, h, max_ttl, datalen);



    return 0;
}
