// HTTP helpers: build a POST request and provide response handlers
#include <stdio.h>    // snprintf, fwrite
#include <stdlib.h>   // malloc, free
#include <string.h>   // strlen, strstr
#include "http.h"    // declarations for HTTP helpers and handler type
#include "util.h"    // utility declarations (not directly used here)

char *build_http_post(const char *host, const char *path, const char *json_body, size_t *out_len) { // build request
    const char *templ =                                                                               // format string
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    size_t body_len = strlen(json_body);                        // compute content length of JSON body
    int needed = snprintf(NULL, 0, templ, path, host, body_len, json_body); // ask snprintf how many bytes needed
    if (needed < 0) return NULL;                                // snprintf error
    size_t req_len = (size_t)needed + 1;                        // add one for the terminating NUL byte
    char *req = malloc(req_len);                                // allocate exact-sized buffer for the request
    if (!req) return NULL;                                      // allocation failed
    int n = snprintf(req, req_len, templ, path, host, body_len, json_body); // produce the full request
    if (n < 0) { free(req); return NULL; }                      // snprintf error: free and abort
    if (out_len) *out_len = (size_t)n;                          // return the number of bytes in the request
    return req;                                                 // return dynamically-allocated request string
}

void handle_raw(const char *data, size_t len) {                 // print the entire HTTP response as-is
    fwrite(data, 1, len, stdout);                               // write bytes directly to stdout
}

void handle_body(const char *data, size_t len) {                // print only the body after the blank line
    const char *p = strstr(data, "\r\n\r\n");               // find the end of headers marker
    if (!p) {                                                   // if no header/body separator found
        fwrite(data, 1, len, stdout);                           // print everything as a fallback
        return;                                                 // and return
    }
    p += 4;                                                     // move pointer past the CRLF-CRLF marker
    fwrite(p, 1, (size_t)(data + len - p), stdout);             // print only the bytes after headers
}
