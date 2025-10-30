#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util.h"

char *iso8601_now_utc(void) {
    time_t t = time(NULL);
    struct tm *tm = gmtime(&t); // simpler than gmtime_r for beginners
    if (!tm) return NULL;
    char buf[32];
    if (strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", tm) == 0) return NULL;
    size_t n = strlen(buf) + 1;
    char *out = malloc(n);
    if (!out) return NULL;
    memcpy(out, buf, n);
    return out;
}

char *build_json_payload(const char *device, const char *iso_time, const char *temperature) {
    // simple and readable: single snprintf with enough space
    const char *templ = "{ \"device\": \"%s\", \"time\": \"%s\", \"temperature\": \"%sC\" }";
    size_t len = strlen(templ) + strlen(device) + strlen(iso_time) + strlen(temperature) + 32;
    char *out = malloc(len);
    if (!out) return NULL;
    snprintf(out, len, templ, device, iso_time, temperature);
    return out;
}
