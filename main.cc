#include "trace.h"
#include <iostream>

struct proto proto_v4 = {.icmpcode    = icmpcode_v4,
                         .recv        = recv_ip4,
                         .sendAddr    = NULL,
                         .receiveAddr = NULL,
                         .lastAddr    = NULL,
                         .bindAddr    = NULL,
                         .addrLen     = 0,
                         .icmpproto   = IPPROTO_ICMP,
                         .ttllevel    = IPPROTO_IP,
                         .ttloptname  = IP_TTL};

#ifdef IPV6
struct proto proto_v6 = {.icmpcode    = icmpcode_v6,
                         .recv        = recv_v6,
                         .sendAddr    = NULL,
                         .receiveAddr = NULL,
                         .lastAddr    = NULL,
                         .bindAddr    = NULL,
                         .addrLen     = 0,
                         .icmpproto   = IPPROTO_ICMP6,
                         .ttllevel    = IPPROTO_IP6,
                         .ttloptname  = IPV6_UNICAST_HOPS};
#endif


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

    ai = host_serv(host, NULL, 0, 0);  // DNS

    h = sock_ntop(ai->ai_addr, ai->ai_addrlen);

    printf("traceroute to %s(%s):%d hops max, %d data bytes\n",
           ai->ai_canonname ? ai->ai_canonname : h, h, max_ttl, datalen);

    if (ai->ai_family == AF_INET) {
        pr = &proto_v4;
#ifdef IPV6
    }
    else if (ai->ai_family == AF_INET6) {
        pr = &proto_v6;
        if (IN6_IS_ADDR_V4MAPPED(
                &(((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr)))
            err_quit("cannot traceroute IPV4-mapped IPV6 address");
#endif
    }
    else
        err_quit("unknown address family %d", ai->ai_family);

    pr->sendAddr    = ai->ai_addr;
    pr->receiveAddr = (struct sockaddr *)calloc(1, ai->ai_addrlen);
    pr->lastAddr    = (struct sockaddr *)calloc(1, ai->ai_addrlen);
    pr->bindAddr    = (struct sockaddr *)calloc(1, ai->ai_addrlen);
    pr->addrLen     = ai->ai_addrlen;

    traceloop();

    return 0;
}
