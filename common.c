// common.c -- Defines some global functions.
//             From JamesM's kernel development tutorials.

#include "common.h"

uint32_t g_tbl[1] = {0xcafebabe };

// Write a byte out to the specified port.
void outb(u16int port, u8int value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8int inb(u16int port)
{
    u8int ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

u16int inw(u16int port)
{
    u16int ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

#if 1
// Copy len bytes from src to dest.
void __attribute((stdcall)) mymemcpy(u8int *dest, const u8int *src, u32int len)
{
    const u8int *sp = (const u8int *)src;
    u8int *dp = (u8int *)dest;
    for (; --len != -1; )
      dp[len] = sp[len];
    //    for(; len != 0; len--) *dp++ = *sp++;
}

// Write len copies of val into dest.
void __attribute__((stdcall)) my_memset(u8int *dest, u8int val, u32int len)
{
    u8int *temp = (u8int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

//void __stdcall mybzero(uint8_t *dest, uint32_t len) 

void __attribute__((stdcall))  mybzero(uint8_t *dest, uint32_t len)  {
  for (; --len != -1; ) {
    dest[len] = 0;
  }
}
#endif

// Compare two strings. Should return -1 if 
// str1 < str2, 0 if they are equal or 1 otherwise.
int my_strcmp(char *str1, char *str2)
{
      int i = 0;
      int failed = 0;
      while(str1[i] != '\0' && str2[i] != '\0')
      {
          if(str1[i] != str2[i])
          {
              failed = 1;
              break;
          }
          i++;
      }
      // why did the loop exit?
      if( (str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0') )
          failed = 1;
  
      return failed;
}

// Copy the NULL-terminated string src into dest, and
// return dest.
char *my_strcpy(char *dest, const char *src)
{
	do
	{
		*dest++ = *src++;
	}
	while (*src != 0);
	return dest;
}

// Concatenate the NULL-terminated string src onto
// the end of dest, and return dest.
char *strcat(char *dest, const char *src)
{
    while (*dest != 0)
    {
        ++dest;
    }

    do
    {
        *dest++ = *src++;
    }
    while (*src != 0);
    return dest;
}
