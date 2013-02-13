#ifndef BYTECODE_DEBUG_H
#define BYTECODE_DEBUG_H

#define DEBUG 0
#define COUNTER 0
#define PROFILE 0

#if DEBUG>=3
#define DEBUG_OUT stdout
#else
#define DEBUG_OUT stderr
#endif

#if DEBUG>=3
#define DEBUG_PRINTF(depth,x,y...) debug_printf(depth, x, ## y)
#else
#define DEBUG_PRINTF(x,y...) 0
#endif

#if DEBUG>=3
int debug_printf(int depth, const char *fmt, ...);
#endif

#endif

