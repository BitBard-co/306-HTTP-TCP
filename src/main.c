#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "client.h"
#include "http.h"
#include "util.h"

int main(int argc, char **argv) {
    if (argc < 7) {
        fprintf(stderr, "Användning: %s <host> <port> <path> <temperatur> <device-uuid> <handler>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    const char *port = argv[2];
    const char *path = argv[3];
    const char *temp = argv[4];
    const char *device = argv[5];
    const char *handler_name = argv[6];

    response_handler_fn handler = handle_raw;
    if (strcmp(handler_name, "body") == 0) handler = handle_body;

    char *ts = iso8601_now_utc();
    if (!ts) return 1;
    char *json = build_json_payload(device, ts, temp);
    free(ts);
    if (!json) return 1;

    size_t req_len = 0;
    char *req = build_http_post(host, path, json, &req_len);
    free(json);
    if (!req) return 1;

    int fd = tcp_connect(host, port);
    if (fd < 0) {
        free(req);
        return 1;
    }

    ssize_t sent = tcp_send_all(fd, req, req_len);
    free(req);
    if (sent < 0) {
        close(fd);
        return 1;
    }

    char *resp = NULL;
    size_t resp_len = 0;
    ssize_t r = tcp_recv_into_buf(fd, &resp, &resp_len);
    close(fd);
    if (r < 0 || !resp) {
        free(resp);
        return 1;
    }

    handler(resp, resp_len);
    free(resp);
    return 0;
}
