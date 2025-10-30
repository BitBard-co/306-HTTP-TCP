#ifndef CLIENT_H                        // include guard start
#define CLIENT_H
#include <stddef.h>                     // size_t declaration
#include <sys/types.h>                  // ssize_t declaration
int tcp_connect(const char *host, const char *port);                 // open TCP connection to host:port
ssize_t tcp_send_all(int fd, const char *buf, size_t len);           // send exactly len bytes (or return -1)
ssize_t tcp_recv_into_buf(int fd, char **out, size_t *out_len);      // recv until close; set *out/*out_len
#endif                                  // include guard end


