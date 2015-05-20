#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

#include <stdio.h>
#include <string.h>
#include "monitor.h"

static void print(const char *data, size_t data_length) {
    size_t idx = 0;
    for (idx = 0; idx < data_length; idx++) {
        putchar((int)((const unsigned char *)data)[idx]);
    }
}

int  printf(const char* format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;
    size_t amount;
    bool rejected_bad_specifier = false;

    while (*format != '\0') {
        if (*format != '%') {
        print_c:
            amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            print(format, amount);
            format += amount;
            written += amount;
            continue;
        }

        const char *format_begun_at = format;

        if (*(++format) == '%')
            goto print_c;

        if (rejected_bad_specifier) {
        incomprehensible_conversion:
          rejected_bad_specifier = true;
          format = format_begun_at;
          goto print_c;
        }
        switch (*format) {
        case 'c':
          {
            format++;
            char c = (char)va_arg(parameters, int /* char promotes to int */);
            print(&c, sizeof(c));
          } break;
        case 's':
          {
            format++;
            const char *s = va_arg(parameters, const char *);
            print(s, strlen(s));
          }
          break;
        case 'x':
          {
            format++;
            int x = (int)va_arg(parameters, int);
            monitor_write_hex(x);
          }
          break;
        default:
          goto incomprehensible_conversion;
        }
    }

    va_end(parameters);

    return written;
}
