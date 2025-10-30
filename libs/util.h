#ifndef UTIL_H
#define UTIL_H
char *iso8601_now_utc(void);
char *build_json_payload(const char *device, const char *iso_time, const char *temperature);
#endif
