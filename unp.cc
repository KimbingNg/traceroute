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


char *sock_ntop(const struct sockaddr *sa, socklen_t salen) {
    char        portstr[8];
    static char str[128];
    switch (sa->sa_family) {
        case AF_INET:
            struct sockaddr_in *sin = (struct sockaddr_in *)sa;

            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return NULL;
            if (ntohs(sin->sin_port) != 0) {
                snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
                strcat(str, portstr);
            }
            return str;
    }
}
