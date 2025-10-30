#ifndef HTTP_H
#define HTTP_H
#include <stddef.h>
typedef void (*response_handler_fn)(const char *data, size_t len);
char *build_http_post(const char *host, const char *path, const char *json_body, size_t *out_len);
void handle_raw(const char *data, size_t len);
void handle_body(const char *data, size_t len);
#endif
