#include <xdg.h>
#include <stdio.h>
#include <basedir.h>

static xdgHandle xdg;

int xdg_init(void){
	const char * const *cdirs,* const *cdir;

	if(xdgInitHandle(&xdg) == NULL){
		fprintf(stderr,"Couldn't initialize XDG data cache\n");
		return -1;
	}
	printf("XDG %.1f [cache: %s] [config: %s]\n",XDG_BASEDIR_SPEC,
			xdgCacheHome(&xdg),xdgConfigHome(&xdg));
	if((cdirs = xdgConfigDirectories(&xdg)) == NULL){
		fprintf(stderr,"Couldn't get XDG config directories\n");
		return -1;
	}
	for(cdir = cdirs ; *cdir ; ++cdir){
		printf("XDG ConfigDirectory: %s\n",*cdir);
	}
	return 0;
}
