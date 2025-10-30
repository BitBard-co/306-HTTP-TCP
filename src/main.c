// Standard headers used by this program
#include <stdio.h>    // fprintf, fwrite
#include <stdlib.h>   // malloc, free, exit codes
#include <string.h>   // strcmp, strlen
#include <time.h>     // time utilities (indirectly via util.c)
#include <unistd.h>   // close()

// Project headers that declare our TCP, HTTP and utility helpers
#include "client.h"  // tcp_connect, tcp_send_all, tcp_recv_into_buf
#include "http.h"    // build_http_post, response_handler_fn, handlers
#include "util.h"    // iso8601_now_utc, build_json_payload

int main(int argc, char **argv) {                              // program entry: receives argument count and values
    if (argc < 7) {                                            // ensure we got all required arguments
        fprintf(stderr, "Användning: %s <host> <port> <path> <temperatur> <device-uuid> <handler>\n", argv[0]);
        return 1;                                              // exit with error if arguments are missing
    }

    const char *host = argv[1];                                // server hostname or IP
    const char *port = argv[2];                                // server TCP port as string
    const char *path = argv[3];                                // HTTP path to POST to
    const char *temp = argv[4];                                // temperature value string
    const char *device = argv[5];                              // device UUID string
    const char *handler_name = argv[6];                        // which response handler to use: raw|body

    response_handler_fn handler = handle_raw;                  // default handler prints full HTTP (headers + body)
    if (strcmp(handler_name, "body") == 0) handler = handle_body; // if user asked for body, switch handler

    char *ts = iso8601_now_utc();                              // build ISO-8601 UTC timestamp string
    if (!ts) return 1;                                         // abort if timestamp allocation failed

    char *json = build_json_payload(device, ts, temp);         // create JSON body with device/time/temperature
    free(ts);                                                  // timestamp no longer needed
    if (!json) return 1;                                       // abort if JSON allocation failed

    size_t req_len = 0;                                        // will receive the byte length of the HTTP request
    char *req = build_http_post(host, path, json, &req_len);   // build an HTTP/1.1 POST request string
    free(json);                                                // JSON body copied into request; free original
    if (!req) return 1;                                        // abort if request allocation failed

    int fd = tcp_connect(host, port);                          // open a TCP connection to host:port
    if (fd < 0) {                                              // if connection failed
        free(req);                                             // free the request buffer
        return 1;                                              // and exit with error
    }

    ssize_t sent = tcp_send_all(fd, req, req_len);             // send the entire request over the socket
    free(req);                                                 // request buffer can be freed after sending
    if (sent < 0) {                                            // if sending failed
        close(fd);                                             // close the socket
        return 1;                                              // exit with error
    }

    char *resp = NULL;                                         // pointer that will receive the response buffer
    size_t resp_len = 0;                                       // total number of bytes received
    ssize_t r = tcp_recv_into_buf(fd, &resp, &resp_len);       // read until the server closes the connection
    close(fd);                                                 // close the socket (Connection: close)
    if (r < 0 || !resp) {                                      // if receiving failed
        free(resp);                                            // free any partial buffer
        return 1;                                              // exit with error
    }

    handler(resp, resp_len);                                   // call the chosen handler function on the data
    free(resp);                                                // release response memory
    return 0;                                                  // indicate success
}
