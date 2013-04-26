#include <xi2.h>
#include <xcb.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

/*static pthread_t tid;

static void *
xi2tid(void *vdisp){
	Display *disp = vdisp;

	XEvent event;
	while(1){
		printf("WE WANT AN XI2 EVENT!\n");
		XNextEvent(disp,&event);
		printf("WE GOT AN XI2 EVENT!\n");
	}
	return NULL;
}*/

static int
enable_xi2_events(Display *x11conn,xcb_window_t win){
	XIEventMask eventmask;
	unsigned char mask[1];

	eventmask.deviceid = XIAllDevices;
	eventmask.mask_len = sizeof(mask);
	eventmask.mask = mask;
	assert(x11conn);
	assert(eventmask.mask);
	XISetMask(mask,XI_ButtonPress);
	XISetMask(mask,XI_Motion);
	XISetMask(mask,XI_KeyPress);
	// FIXME
	assert(win != DefaultRootWindow(x11conn));
	if(XISelectEvents(x11conn,win,&eventmask,1)){
		fprintf(stderr,"Couldn't select XI2 events\n");
		return -1;
	}
	//pthread_create(&tid,NULL,xi2tid,x11conn);
	return 0;
}

int init_xi2(Display *x11conn,xcb_window_t win){
	int xi_opcode,event,err;
	int major,minor;

	if(!XQueryExtension(x11conn,"XInputExtension",&xi_opcode,&event,&err)){
		fprintf(stderr,"X Input extension is unavailable\n");
		return -1;
	}
	major = 2;
	minor = 0;
	if(XIQueryVersion(x11conn,&major,&minor) != Success){
		fprintf(stderr,"XI2 not available (server supports %d.%d)\n",
				major,minor);
		return -1;
	}
	printf("Using XInputExtension %d.%d\n",major,minor);
	if(enable_xi2_events(x11conn,win)){
		return -1;
	}
	return 0;
}
