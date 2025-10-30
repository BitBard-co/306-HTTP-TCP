// Utility helpers to build timestamps and JSON strings
#include <stdio.h>    // snprintf
#include <stdlib.h>   // malloc, free
#include <string.h>   // memcpy, strlen
#include <time.h>     // time, gmtime, strftime
#include "util.h"    // declarations for these helpers

char *iso8601_now_utc(void) {                                  // return current UTC time as ISO-8601 string
    time_t t = time(NULL);                                      // get current time in seconds since epoch
    struct tm *tm = gmtime(&t);                                 // convert to UTC calendar time (broken-down)
    if (!tm) return NULL;                                       // conversion failed
    char buf[32];                                               // temporary stack buffer to format into
    if (strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", tm) == 0) return NULL; // format as ISO-8601
    size_t n = strlen(buf) + 1;                                 // compute space needed including NUL
    char *out = malloc(n);                                      // allocate exact-sized heap buffer
    if (!out) return NULL;                                      // allocation failed
    memcpy(out, buf, n);                                        // copy the formatted string including NUL
    return out;                                                 // return heap-allocated string to caller
}

char *build_json_payload(const char *device, const char *iso_time, const char *temperature) { // JSON builder
    const char *templ = "{ \"device\": \"%s\", \"time\": \"%s\", \"temperature\": \"%sC\" }"; // format
    size_t len = strlen(templ) + strlen(device) + strlen(iso_time) + strlen(temperature) + 32; // rough capacity
    char *out = malloc(len);                               // allocate buffer big enough for formatted JSON
    if (!out) return NULL;                                 // allocation failed
    snprintf(out, len, templ, device, iso_time, temperature); // format JSON into buffer
    return out;                                            // return heap string containing the JSON document
}
