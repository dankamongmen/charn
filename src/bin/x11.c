#include <x11.h>
#include <stdio.h>
#include <X11/Xlib.h>

static int
x11_error_handler(Display *dpy,XErrorEvent *xee){
	// FIXME need thread-safety!
	if(!dpy || !xee){
		fprintf(stderr,"XError with NULL display/xerrorevent\n");
		return -1;
	}
	// FIXME format dpy/xee for display
	return 0;
}

// Xlib exits no matter the return value from this function
static int
x11_fatal_handler(Display *dpy){
	if(!dpy){
		fprintf(stderr,"XIOError with NULL display\n");
		return -1;
	}
	// FIXME format dpy for display
	return 0;
}

Display *init_x11(void){
	Display *dpy;

	printf("Opening connection to %s...\n",XDisplayName(NULL));
	if((dpy = XOpenDisplay(NULL)) == NULL){
		fprintf(stderr,"Couldn't open connection to X11 server\n");
		return NULL;
	}
	XSetErrorHandler(x11_error_handler);
	XSetIOErrorHandler(x11_fatal_handler);
	return dpy;
}
