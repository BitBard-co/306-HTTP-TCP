// Very small TCP helper functions: connect, send all, receive all.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

int tcp_connect(const char *host, const char *port) {
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rv = getaddrinfo(host, port, &hints, &res);
    if (rv != 0) return -1;

    int fd = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) continue;
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(fd);
            fd = -1;
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    return fd;
}

ssize_t tcp_send_all(int fd, const char *buf, size_t len) {
    size_t sent_total = 0;
    while (sent_total < len) {
        ssize_t n = send(fd, buf + sent_total, len - sent_total, 0);
        if (n <= 0) return -1;
        sent_total += (size_t)n;
    }
    return (ssize_t)sent_total;
}

ssize_t tcp_recv_into_buf(int fd, char **out, size_t *out_len) {
    size_t cap = 4096;             // start small
    size_t used = 0;
    char *buf = malloc(cap);
    if (!buf) return -1;
    for (;;) {
        if (used == cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) { free(buf); return -1; }
            buf = nb;
            cap = ncap;
        }
        ssize_t n = recv(fd, buf + used, cap - used, 0);
        if (n < 0) { free(buf); return -1; }
        if (n == 0) break; // done
        used += (size_t)n;
    }
    *out = buf;
    *out_len = used;
    return (ssize_t)used;
}
