#ifndef CLIENT_H
#define CLIENT_H
#include <stddef.h>
int tcp_connect(const char *host, const char *port);
ssize_t tcp_send_all(int fd, const char *buf, size_t len);
ssize_t tcp_recv_into_buf(int fd, char **out, size_t *out_len);
#endif


