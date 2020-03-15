/*************************************************************************
    > File Name: sig_alarm.cc
    > Author: ngkimbing
    > Mail: ngkimbing@foxmail.com
    > Created Time: Sun 15 Mar 2020 05:01:43 PM CST
 ************************************************************************/

#include "trace.h"

int gotalarm;

void sig_alarm(int sogno) {
    gotalarm = 1;
    return;
}
