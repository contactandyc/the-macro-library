#ifndef _macro_to_H
#define _macro_to_H

#include <stdio.h>

static inline int macro_to_int(const char *str, int default_value) {
    if (!str) return default_value;

    int num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline int32_t macro_to_int32(const char *str, int32_t default_value) {
    if (!str) return default_value;

    int32_t num = 0;

    int ch = *str;
    if(*str == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline uint32_t macro_to_uint32(const char *str, uint32_t default_value) {
    if (!str) return default_value;

    uint32_t num = 0;

    int ch = *str;
    while (ch) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            str++;
            ch = *str;
            continue;
        }
        // Not a valid digit
        return default_value;
    }

    return num;
}


static inline int64_t macro_to_int64(const char *str, int64_t default_value) {
    if (!str) return default_value;

    int64_t num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline uint64_t macro_to_uint64(const char *str, uint64_t default_value) {
    if (!str) return default_value;

    uint64_t num = 0;

    int ch = *str;
    while (ch) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            str++;
            ch = *str;
            continue;
        }
        // Not a valid digit
        return default_value;
    }

    return num;
}

static inline float macro_to_float(const char *str, float default_value) {
    if (!str) return default_value;

    float r;
    if(sscanf(str, "%f", &r) == 1)
        return r;
    return default_value;
}

static inline double macro_to_double(const char *str, double default_value) {
    if (!str) return default_value;

    double r;
    if(sscanf(str, "%lf", &r) == 1)
        return r;
    return default_value;
}

static inline bool macro_to_bool(const char *str, bool default_value) {
    if (!str) return default_value;

    if(default_value == true) {
        if(str[0] == 'f' || str[0] == '0' || str[0] == 'n' || str[0] == 'F' || str[0] == 'N')
            return false;
        return true;
    }
    else {
        if(str[0] == 't' || str[0] == '1' || str[0] == 'y' || str[0] == 'T' || str[0] == 'Y')
            return true;
        return false;
    }
}

#endif