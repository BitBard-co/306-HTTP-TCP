#ifndef HTTP_H                                                       // include guard start
#define HTTP_H
#include <stddef.h>                                                  // size_t declaration
typedef void (*response_handler_fn)(const char *data, size_t len);   // function pointer type for response printers
char *build_http_post(const char *host, const char *path, const char *json_body, size_t *out_len); // build POST
void handle_raw(const char *data, size_t len);                       // print full HTTP response (headers+body)
void handle_body(const char *data, size_t len);                      // print only HTTP body
#endif                                                               // include guard end
