#include <xdg.h>
#include <stdio.h>
#include <basedir.h>

static xdgHandle xdg;

int xdg_init(void){
	if(xdgInitHandle(&xdg) == NULL){
		fprintf(stderr,"Couldn't initialize XDG data cache\n");
		return -1;
	}
	printf("XDG [cache: %s] [config: %s]\n",xdgCacheHome(&xdg),xdgConfigHome(&xdg));
	return 0;
}
