#include <xi2.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

int init_xi2(Display *x11conn){
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
	return 0;
}
