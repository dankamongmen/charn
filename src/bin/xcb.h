#ifndef CHARN_XCB
#define CHARN_XCB

#include <xcb/xcb.h>
#include <X11/Xlib.h>

xcb_window_t xcb_init(Display *);
int xcb_stop(void);

int set_title(xcb_window_t,const char *);

#endif
