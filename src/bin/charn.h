#ifndef CHARN_CHARN
#define CHARN_CHARN

#include <stdio.h>
#include <stdarg.h>

// Global "read-only" state, set during startup
extern int Verbose; // whether to print verbose diagnostics
extern int ErrorSoftfail; // if non-0, don't call through to Xlib error handler

// Diagnostics
static inline int
Vfprintf(FILE *f, const char *fmt, ...){
	int ret;

	if(Verbose){
		va_list args;

		va_start(args, fmt);
		ret = vfprintf(f, fmt, args);
		va_end(args);
	}
	return ret;
}

#endif
