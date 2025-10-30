// Small TCP helper functions implemented in C
#include <stdio.h>       // general I/O utilities (optional here)
#include <stdlib.h>      // malloc, free
#include <string.h>      // memset
#include <unistd.h>      // close()
#include <sys/types.h>   // sockets types
#include <sys/socket.h>  // socket(), connect(), send(), recv()
#include <netdb.h>       // getaddrinfo(), freeaddrinfo(), struct addrinfo
#include <errno.h>       // errno codes

int tcp_connect(const char *host, const char *port) {         // open a TCP connection to host:port
    struct addrinfo hints, *res, *p;                           // addrinfo structures for resolution and iteration
    memset(&hints, 0, sizeof hints);                          // start with a clean hints struct
    hints.ai_family = AF_UNSPEC;                               // allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;                           // TCP stream sockets

    int rv = getaddrinfo(host, port, &hints, &res);            // resolve host and service into address list
    if (rv != 0) return -1;                                    // resolution failed

    int fd = -1;                                               // will hold the connected socket descriptor
    for (p = res; p != NULL; p = p->ai_next) {                 // try each candidate address until one connects
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol); // create a socket for this address
        if (fd == -1) continue;                                // try next if socket creation failed
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {    // attempt to connect
            close(fd);                                         // close failed socket
            fd = -1;                                           // reset descriptor
            continue;                                          // try the next address
        }
        break;                                                 // success: stop iterating
    }
    freeaddrinfo(res);                                         // free address list memory
    return fd;                                                 // return connected socket (or -1 if none worked)
}

ssize_t tcp_send_all(int fd, const char *buf, size_t len) {   // send exactly len bytes unless an error occurs
    size_t sent_total = 0;                                     // how many bytes sent so far
    while (sent_total < len) {                                 // loop until we've sent the entire buffer
        ssize_t n = send(fd, buf + sent_total, len - sent_total, 0); // attempt to send remaining bytes
        if (n <= 0) return -1;                                 // error or connection closed
        sent_total += (size_t)n;                               // advance by number of bytes actually sent
    }
    return (ssize_t)sent_total;                                // report total bytes sent
}

ssize_t tcp_recv_into_buf(int fd, char **out, size_t *out_len) { // read until peer closes; allocate enough space
    size_t cap = 4096;                                           // initial buffer capacity in bytes
    size_t used = 0;                                             // how many bytes filled so far
    char *buf = malloc(cap);                                     // allocate buffer
    if (!buf) return -1;                                         // allocation failure
    for (;;) {                                                   // keep receiving until the socket closes
        if (used == cap) {                                       // grow buffer if full
            size_t ncap = cap * 2;                               // double the capacity
            char *nb = realloc(buf, ncap);                       // attempt to resize
            if (!nb) { free(buf); return -1; }                   // on failure, free and abort
            buf = nb;                                            // use the new buffer pointer
            cap = ncap;                                          // update capacity
        }
        ssize_t n = recv(fd, buf + used, cap - used, 0);        // read into the free tail of the buffer
        if (n < 0) { free(buf); return -1; }                     // read error
        if (n == 0) break;                                       // peer closed: done reading
        used += (size_t)n;                                       // account for bytes just received
    }
    *out = buf;                                                  // return pointer to caller via pointer-to-pointer
    *out_len = used;                                             // return total bytes read
    return (ssize_t)used;                                        // report number of bytes read
}
