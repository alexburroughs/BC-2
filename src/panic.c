#include "panic.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void panic (const char* msg, ...)
{
    va_list args;
    fprintf( stderr, "Error: " );
    va_start( args, msg);
    vfprintf( stderr, msg, args );
    va_end( args );
    fprintf( stderr, "\n" );

    exit(1);
}

void debug (const char* msg, ...)
{
    va_list args;
    fprintf( stderr, "Debug: " );
    va_start( args, msg);
    vfprintf( stderr, msg, args );
    va_end( args );
    fprintf( stderr, "\n" );
}