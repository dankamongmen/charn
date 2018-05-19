#include "charn.h"
#include "config.h"
#include <x11.h>
#include <xi2.h>
#include <xcb.h>
#include <xdg.h>
#include <glx.h>
#include <glew.h>
#include <errno.h>
#include <stdio.h>
#include <epoll.h>
#include <getopt.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

int Verbose;
int ErrorSoftfail;

static void
usage(const char *name,int status){
	FILE *fp = status ? stderr : stdout;

	fprintf(fp, "usage: %s [ options ]\n",name);
	fprintf(fp, "options:\n");
	fprintf(fp, "\t-e/--error: don't exit on soft X errors\n");
	fprintf(fp, "\t-h/--help: display this help text\n");
	fprintf(fp, "\t-v/--verbose: increase verbosity\n");
	exit(status);
}

int main(int argc,char **argv){
	const struct option longopts [] = {
		{ "help",	0,	NULL,		'h'	},
		{ "verbose",	0,	NULL,		'v'	},
		{ "error",	0,	NULL,		'e'	},
		{ NULL,		0,	NULL,		0	}
	};
	xcb_window_t wid;
	int opt,longopt;
	CharnConfig cc;
	Display *dpy;

	if((setlocale(LC_ALL,NULL)) == NULL){
		fprintf(stderr,"Couldn't setlocale(LC_ALL)\n");
		return EXIT_FAILURE;
	}
	printf("%s\n", PACKAGE_STRING);
	Verbose = 0;
	ErrorSoftfail = 0;
	// FIXME implement -r/--replace option
	while((opt = getopt_long(argc, argv, "hve", longopts, &longopt)) >= 0){
		switch(opt){
			case 'h':
				usage(argv[0], EXIT_SUCCESS);
				break;
			case 'v':
				Verbose = 1;
				break;
			case 'e':
				ErrorSoftfail = 1;
				printf("Disabling X error passthrough\n");
				break;
			default:
				usage(argv[0], EXIT_FAILURE);
				break;
		}
	}
	if(init_config(&cc)){
		return EXIT_FAILURE;
	}
	if(xdg_init(&cc)){
		return EXIT_FAILURE;
	}
	if((dpy = init_x11()) == NULL){
		return EXIT_FAILURE;
	}
	if(event_init()){
		return EXIT_FAILURE;
	}
	if((wid = xcb_init(NULL)) == (xcb_window_t)-1){
		return EXIT_FAILURE;
	}
	if(init_xi2(dpy,wid)){
		return EXIT_FAILURE;
	}
	if(init_glx(dpy,wid)){
		return EXIT_FAILURE;
	}
	if(glew_init()){
		return EXIT_FAILURE;
	}
	if(set_title(wid,PACKAGE_NAME)){
		return EXIT_FAILURE;
	}
	if(event_loop()){
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
