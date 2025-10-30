#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "util.h"

char *build_http_post(const char *host, const char *path, const char *json_body, size_t *out_len) {
    size_t body_len = strlen(json_body);
    char content_len[64];
    snprintf(content_len, sizeof content_len, "%zu", body_len);

    const char *templ = 
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "Content-Length: %s\r\n"
        "\r\n"
        "%s";

    size_t req_len = strlen(templ) + strlen(path) + strlen(host) + strlen(content_len) + strlen(json_body) + 1;
    char *req = malloc(req_len);
    if (!req) return NULL;
    int n = snprintf(req, req_len, templ, path, host, content_len, json_body);
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
