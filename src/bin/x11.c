#include <x11.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <charn.h>

static int (*old_error_handler)(Display *, XErrorEvent *);
static int (*old_ioerror_handler)(Display *);

static int
x11_error_handler(Display *dpy,XErrorEvent *xee){
	char errorstring[128]; // FIXME can we do better? ugh
	// FIXME need thread-safety!
	if(!dpy || !xee){
		fprintf(stderr,"XError with NULL display/xerrorevent\n");
		return -1;
	}
	// FIXME format dpy
	XGetErrorText(dpy, xee->error_code, errorstring, sizeof(errorstring));
	fprintf(stderr, "XError %d (req %lu, %d-%d): %s\n", xee->error_code,
		xee->serial, xee->request_code, xee->minor_code, errorstring);
	// Xlib default error handler terminates the process :/ It would be
	//  nice to get its detailed output, though FIXME
	if(ErrorSoftfail || !old_error_handler){
		return 0;
	}
	return old_error_handler(dpy, xee);
}

// Xlib exits no matter the return value from this function
static int
x11_fatal_handler(Display *dpy){
	if(!dpy){
		fprintf(stderr,"XIOError with NULL display\n");
		return -1;
	}
	// FIXME format dpy for display
	return old_ioerror_handler ? old_ioerror_handler(dpy) : 0;
}

Display *init_x11(void){
	Display *dpy;

	printf("Opening connection to %s...\n",XDisplayName(NULL));
	if((dpy = XOpenDisplay(NULL)) == NULL){
		fprintf(stderr,"Couldn't open connection to X11 server\n");
		return NULL;
	}
	old_error_handler = XSetErrorHandler(x11_error_handler);
	old_ioerror_handler = XSetIOErrorHandler(x11_fatal_handler);
	return dpy;
}
