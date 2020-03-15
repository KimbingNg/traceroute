//
// Created by Kimbing Ng on 3/15/20.
//

#include "traceutil.h"
#include "sig_alarm.h"
#include "trace.h"
#include "unp.h"

int recv_ip4(int seq, struct timeval *tv) {
    struct ip *      ip1, *ip2;
    struct icmp *    icmp;
    struct udphdr *  udp;
    int              iphlen1, iphlen2, icmplen, ret, n;
    socklen_t        len;
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_alarm;
    act.sa_flags   = 0;
    sigaction(SIGALRM, &act, NULL);


    gotalarm = FALSE;
    alarm(3);
    for (;;) {
        if (gotalarm) {
            ret = TRACE_RESULT_TIMEOUT;
            break;
        }
        len = pr->addrLen;
        n   = recvfrom(recvfd, recvbuf, sizeof(recvbuf), 0, pr->receiveAddr,
                     &len);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror("recv error");
                exit(-1);
            }
        }

        ip1     = (struct ip *)recvbuf;
        iphlen1 = ip1->ip_hl << 2;

        icmp = (struct icmp *)(recvbuf + iphlen1);
        if ((icmplen = n - iphlen1) < ICMP_HLEN)
            continue;

        if (icmp->icmp_type == ICMP_TIMXCEED &&
            icmp->icmp_code == ICMP_TIMXCEED_INTRANS) {
            if (icmplen < ICMP_HLEN + sizeof(struct ip))
                continue;

            ip2     = (struct ip *)(recvbuf + iphlen1 + ICMP_HLEN);
            iphlen2 = ip2->ip_hl << 2;

            if (icmplen < ICMP_HLEN + iphlen2 + UDP_HLEN)
                continue;

            udp = (struct udphdr *)(recvbuf + iphlen1 + ICMP_HLEN + iphlen2);
            if (ip2->ip_p == IPPROTO_UDP && udp->source == htons(sport) &&
                udp->dest == htons(dport + seq)) {
                ret = TRACE_RESULT_TIMEEXCEED;
                break;
            }
        }
        else if (icmp->icmp_type == ICMP_UNREACH) {
            if (icmplen < ICMP_HLEN + sizeof(struct ip))
                continue;

            ip2     = (struct ip *)(recvbuf + iphlen1 + ICMP_HLEN);
            iphlen2 = ip2->ip_hl << 2;
            if (icmplen < ICMP_HLEN + iphlen2 + UDP_HLEN)
                continue;

            udp = (struct udphdr *)(recvbuf + iphlen1 + ICMP_HLEN + iphlen2);
            if (ip2->ip_p == IPPROTO_UDP && udp->source == htons(sport) &&
                udp->dest == htons(dport + seq)) {
                if (icmp->icmp_code == ICMP_UNREACH_PORT)
                    ret = TRACE_RESULT_UNREACH;
                else
                    ret = icmp->icmp_code; /* 0, 1, 2 ... */
                break;
            }
        }
    }
    alarm(0);
    gettimeofday(tv, NULL);

    return ret;
}

const char *icmpcode_v4(int a) {}
