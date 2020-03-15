//
// Created by Kimbing Ng on 3/15/20.
//

#ifndef MYTRACEROUTE_TRACEUTIL_H
#define MYTRACEROUTE_TRACEUTIL_H

int recv_ip4(int seq, struct timeval *tv);

const char *icmpcode_v4(int);
const char *icmpcode_v6(int);

#endif  // MYTRACEROUTE_TRACEUTIL_H
