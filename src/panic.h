#ifndef PANIC_H
#define PANIC_H

#define DEBUG_COMPILER

#ifdef DEBUG_COMPILER
#define DEBUG(x, ...) debug(x, ...);
#else
#define DEBUG(x, ...) 
#endif

void panic(const char* msg, ...);
void debug(const char* msg, ...);

#endif