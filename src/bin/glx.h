#ifndef CHARN_GLX
#define CHARN_GLX

#include <xcb/xcb.h>
#include <X11/Xlib.h>

int init_glx(Display *,xcb_window_t);

#endif
