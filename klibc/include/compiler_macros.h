#ifndef COMPILER_MACROS_H
#define COMPILER_MACROS_H

#if defined __GNUC__
#define STDCALL __attribute__((stdcall))
#define FMT_PRINTF(x,y) __attribute__((format(printf,x,y)))
#else
#define STDCALL
#define FMT_PRINTF(x,y)
#endif

#endif /* COMPILER_MACROS_H */
