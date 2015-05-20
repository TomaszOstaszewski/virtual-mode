#include <string.h>

void *memset(void *bufptr, int value, size_t size) {
    size_t i;
    unsigned char *buf = (unsigned char *)bufptr;
    for (i = 0; i < size; i++)
        buf[i] = (unsigned char)value;
    return bufptr;
}
