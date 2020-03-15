//
// Created by Kimbing Ng on 3/15/20.
//

#include "trace.h"

int           datalen = sizeof(struct rec);
int           max_ttl = 30;
int           nprobes = 3;
u_short       dport   = 32768 + 666;
int           verbose;
pid_t         pid;
struct proto *pr;

char *  host;
u_short sport;
int     nsent;
int     probe;
int     sendfd, recvfd;
int     ttl;

char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];
void traceloop(void) {
    int            seq, code, done;
    double         rtt;
    struct rec *   rec;
    struct timeval tvrecv;

    recvfd = socket(pr->sendAddr->sa_family, SOCK_RAW, pr->icmpproto);
    if (recvfd < 0)
        error_exit("recvfd socket");

    setuid(getuid());

#ifdef IPV6
    if (pr->sendAddr->sa_family == AF_INET6 && !verbose) {
        // TODO
    }
#endif
    sendfd                  = socket(pr->sendAddr->sa_family, SOCK_DGRAM, 0);
    pr->bindAddr->sa_family = pr->sendAddr->sa_family;  // ????

    sport = (getuid() & 0xffff) | 0x8000;  // UDP port

    sockSetPort(pr->bindAddr, sport);

    bind(sendfd, pr->bindAddr, pr->addrLen);

    sig_alarm(SIGALRM);

    seq = 0, done = 0;
    for (ttl = 1; ttl <= max_ttl && !done; ++ttl) {
        if (setsockopt(sendfd, pr->ttllevel, pr->ttloptname, &ttl,
                       sizeof(ttl)) == -1)
            error_exit("setsockopt");
        bzero(pr->lastAddr, pr->addrLen);
        printf("%2d ", ttl);
        fflush(stdout);

        for (probe = 0; probe < nprobes; ++probe) {
            rec          = (struct rec *)sendbuf;
            rec->rec_seq = seq;
            rec->rec_ttl = ttl;
            gettimeofday(&rec->rec_tv, NULL);
            sockSetPort(pr->sendAddr, dport + seq);

            if (sendto(sendfd, sendbuf, datalen, 0, pr->sendAddr, pr->addrLen) <
                0) {
                error_exit("sendto");
            }

            if ((code = (*pr->recv)(seq, &tvrecv)) == TRACE_RESULT_TIMEOUT)
                printf(" *");  // timeout
            else {
                char str[NI_MAXHOST];
                if (sock_cmp_addr(pr->receiveAddr, pr->lastAddr) != 0) {
                    // don't resolve the name
                    // if (getnameinfo(pr->receiveAddr, pr->addrLen, str,
                    // sizeof(str),NULL, 0, 0) == 0)
                    printf(" %s", sock_ntop(pr->receiveAddr, pr->addrLen));
                    memcpy(pr->lastAddr, pr->receiveAddr, pr->addrLen);
                }
                tv_sub(&tvrecv, &rec->rec_tv);
                rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
                printf("  %.3f ms", rtt);

                if (code == TRACE_RESULT_UNREACH)
                    ++done;
                else if (code >= 0)
                    printf(" (ICMP %s)", (*pr->icmpcode)(code));
                // else {
                //    printf("unknow code: %d\n", code);
                //}
            }
            fflush(stdout);
        }
        printf("\n");
    }
}
