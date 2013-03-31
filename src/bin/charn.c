#include <xcb.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <basedir.h>

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
	xdgHandle xdg;
	int xcbfd;

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
	if(xdgInitHandle(&xdg) == NULL){
		fprintf(stderr,"Couldn't initialize XDG data cache\n");
		return EXIT_FAILURE;
	}
	printf("XDG [cache: %s] [config: %s]\n",xdgCacheHome(&xdg),xdgConfigHome(&xdg));
	if((xcbfd = xcb_init()) <= 0){
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
