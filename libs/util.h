#ifndef UTIL_H                                                   // include guard start
#define UTIL_H
char *iso8601_now_utc(void);                                    // allocate and return current UTC time string
char *build_json_payload(const char *device, const char *iso_time, const char *temperature); // build JSON body
#endif                                                           // include guard end
