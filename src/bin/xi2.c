#include <xi2.h>
#include <stdio.h>
#include <X11/Xlib.h>

int init_xi2(Display *x11conn){
	int xi_opcode,event,err;

	if(!XQueryExtension(x11conn,"XInputExtension",&xi_opcode,&event,&err)){
		fprintf(stderr,"X Input extension is unavailable\n");
		return -1;
	}
	return 0;
}
