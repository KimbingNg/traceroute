#ifndef __unp_h
#define __unp_h

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <cstdarg>
#include <syslog.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sys/wait.h>
// #include<readline/readline.h>
#define MAXLINE 64

struct sockaddr_in makeAddr(char const *addr, uint16_t port);

void err_quit(const char *, ...);

void err_msg(const char *);

typedef struct sockaddr SA;

typedef void (*SigFunc)(int);

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype);

static void err_doit(int errnoflag, int level, const char *fmt, va_list ap);

char *sock_ntop(const struct sockaddr *sa, socklen_t salen);
#endif
