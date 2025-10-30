// Build a minimal HTTP/1.1 POST request. Keep it simple and readable.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "util.h"

char *build_http_post(const char *host, const char *path, const char *json_body, size_t *out_len) {
    const char *templ =
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    size_t body_len = strlen(json_body);
    // First pass: get required length
    int needed = snprintf(NULL, 0, templ, path, host, body_len, json_body);
    if (needed < 0) return NULL;
    size_t req_len = (size_t)needed + 1; // include NUL
    char *req = malloc(req_len);
    if (!req) return NULL;
    int n = snprintf(req, req_len, templ, path, host, body_len, json_body);
    if (n < 0) { free(req); return NULL; }
    if (out_len) *out_len = (size_t)n;
    return req;
}

void handle_raw(const char *data, size_t len) {
    fwrite(data, 1, len, stdout);
}

void handle_body(const char *data, size_t len) {
    const char *p = strstr(data, "\r\n\r\n");
    if (!p) {
        fwrite(data, 1, len, stdout);
        return;
    }
    p += 4;
    fwrite(p, 1, (size_t)(data + len - p), stdout);
}
