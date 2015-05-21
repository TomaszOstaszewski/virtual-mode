#ifndef COMPILER_MACROS_H
#define COMPILER_MACROS_H

#if defined __GNUC__
#define STDCALL __attribute__((stdcall))
#define CDECL __attribute__((cdecl))
#define FMT_PRINTF(x,y) __attribute__((format(printf,x,y)))
#define PACKED __attribute__((packed))
#else
#define STDCALL
#define FMT_PRINTF(x,y)
#define PACKED
#endif

#endif /* COMPILER_MACROS_H */
