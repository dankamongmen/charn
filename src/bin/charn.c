#include <xi2.h>
#include <xcb.h>
#include <xdg.h>
#include <dbus.h>
#include <errno.h>
#include <stdio.h>
#include <epoll.h>
#include <getopt.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

static void
usage(const char *name,int status){
	FILE *fp = status ? stderr : stdout;

	fprintf(fp,"usage: %s [ options ]\n",name);
	fprintf(fp,"options:\n");
	fprintf(fp,"\t-h/--help: display this help text\n");
	exit(status);
}

int main(int argc,char **argv){
	const struct option longopts [] = {
		{ "help",	0,	NULL,		'h'	},
		{ NULL,		0,	NULL,		0	}
	};
	int opt,longopt;
	Display *dpy;

	if((setlocale(LC_ALL,NULL)) == NULL){
		fprintf(stderr,"Couldn't setlocale(LC_ALL)\n");
		return EXIT_FAILURE;
	}
	while((opt = getopt_long(argc,argv,"h",longopts,&longopt)) >= 0){
		switch(opt){
			case 'h':
				usage(argv[0],EXIT_SUCCESS);
				break;
			default:
				usage(argv[0],EXIT_FAILURE);
				break;
		}
	}
	if((dpy = XOpenDisplay(NULL)) == NULL){
		fprintf(stderr,"Couldn't open connection to X11 server\n");
		return EXIT_FAILURE;
	}
	if(event_init()){
		return EXIT_FAILURE;
	}
	if(xcb_init(NULL)){
		return EXIT_FAILURE;
	}
	if(init_xi2(dpy)){
		return EXIT_FAILURE;
	}
	if(dbus_init()){
		return EXIT_FAILURE;
	}
	system("gnome-terminal");
	if(event_loop()){
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
