#include <xdg.h>
#include <stdio.h>
#include <basedir.h>

static xdgHandle xdg;

int xdg_init(void){
	if(xdgInitHandle(&xdg) == NULL){
		fprintf(stderr,"Couldn't initialize XDG data cache\n");
		return -1;
	}
	printf("XDG %.1f [cache: %s] [config: %s]\n",XDG_BASEDIR_SPEC,
			xdgCacheHome(&xdg),xdgConfigHome(&xdg));
	return 0;
}
