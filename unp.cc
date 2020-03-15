#include "unp.h"

static int daemon_proc;

struct sockaddr_in makeAddr(char const *addr, uint16_t port) {
    struct sockaddr_in ret {};
    inet_pton(AF_INET, addr, &ret.sin_addr.s_addr);
    // ret.sin_addr.s_addr = inet_addr(addr);
    ret.sin_family = AF_INET;
    ret.sin_port   = htons(port);
    bzero(&ret.sin_zero, 8);
    return ret;
}


void err_quit(const char *__s, ...) {
    va_list ap;
    va_start(ap, __s);
    err_doit(0, LOG_ERR, __s, ap);
    va_end(ap);


    // fprintf(stderr, __s, ++k);
    // exit(-1);
}

static void err_doit(int errnoflag, int level, const char *fmt, va_list ap) {
    fprintf(stderr, "err_doit\n");

    int  errno_save, n;
    char buf[MAXLINE + 1];
    errno_save = errno;
    vsnprintf(buf, MAXLINE, fmt, ap);

    n = strlen(buf);

    if (errnoflag)
        snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
    strcat(buf, "\n");

    if (daemon_proc) {
        syslog(level, buf);
    }
    else {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
    }
    return;
}

void err_msg(const char *__s) {
    fprintf(stderr, __s);
}

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype) {
    int             n;
    struct addrinfo hints, *res;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags    = AI_CANONNAME;
    hints.ai_family   = family;
    hints.ai_socktype = socktype;

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
        return NULL;

    return res;
}


char *sock_ntop(const struct sockaddr *addr, socklen_t salen) {
    static char str[64];

    switch (addr->sa_family) {
        case AF_INET: {
            struct sockaddr_in *sin = (struct sockaddr_in *)addr;
            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return NULL;
            break;
        }

        case AF_INET6: {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)addr;
            if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
                return NULL;
            break;
        }
    }

    return str;
}


void sockSetPort(struct sockaddr *sa, int port) {
    ((struct sockaddr_in *)sa)->sin_port = htons(port);
}

void error_exit(const char *s) {
    perror(s);
    exit(-1);
}


int sock_cmp_addr(const struct sockaddr *a1, const struct sockaddr *a2) {
    if (a1->sa_family != a2->sa_family)
        return -1;

    switch (a1->sa_family) {
        case AF_INET:
            return memcmp(&((struct sockaddr_in *)a1)->sin_addr,
                          &((struct sockaddr_in *)a2)->sin_addr,
                          sizeof(struct in_addr));

        case AF_INET6:
            return memcmp(&((struct sockaddr_in6 *)a1)->sin6_addr,
                          &((struct sockaddr_in6 *)a2)->sin6_addr,
                          sizeof(struct in6_addr));
    }

    return -1;
}

void tv_sub(struct timeval *tv1, struct timeval *tv2) {
    if ((tv1->tv_usec -= tv2->tv_usec) < 0) {
        --tv1->tv_sec;
        tv1->tv_usec += 1000000;
    }
    tv1->tv_sec -= tv2->tv_sec;
}
