#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util.h"

char *iso8601_now_utc(void) {
    time_t t = time(NULL);
    struct tm tm;
    if (!gmtime_r(&t, &tm)) return NULL;
    char buf[32];
    if (strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", &tm) == 0) return NULL;
    char *out = malloc(strlen(buf) + 1);
    if (!out) return NULL;
    strcpy(out, buf);
    return out;
}

char *build_json_payload(const char *device, const char *iso_time, const char *temperature) {
    const char *templ = "{ \"device\": \"%s\", \"time\": \"%s\", \"temperature\": \"%s°C\" }";
    size_t len = strlen(templ) + strlen(device) + strlen(iso_time) + strlen(temperature) + 1 + 16;
    char *out = malloc(len);
    if (!out) return NULL;
    snprintf(out, len, templ, device, iso_time, temperature);
    return out;
}

char *str_dup_range(const char *start, const char *end) {
    if (!start || !end || end < start) return NULL;
    size_t len = (size_t)(end - start);
    char *s = malloc(len + 1);
    if (!s) return NULL;
    memcpy(s, start, len);
    s[len] = '\0';
    return s;
}
