#include <xdg.h>
#include <stdio.h>
#include <basedir.h>
#include <config.h>

static const char DEFAULT_CONFIG_NAME[] = "charn/config";

static xdgHandle xdg;

int xdg_init(CharnConfig *cc){
	const char * const *cdirs,* const *cdir;

	if(xdgInitHandle(&xdg) == NULL){
		fprintf(stderr,"Couldn't initialize XDG data cache\n");
		return -1;
	}
	const char *config_home = xdgConfigHome(&xdg);
	printf("XDG %.1f [cache: %s] [config: %s]\n", XDG_BASEDIR_SPEC,
			xdgCacheHome(&xdg), config_home);
	load_config(cc, config_home, DEFAULT_CONFIG_NAME);
	if((cdirs = xdgConfigDirectories(&xdg)) == NULL){
		fprintf(stderr,"Couldn't get XDG config directories\n");
		return -1;
	}
	for(cdir = cdirs ; *cdir ; ++cdir){
		printf("XDG ConfigDirectory: %s\n", *cdir);
		load_config(cc, *cdir, DEFAULT_CONFIG_NAME);
	}
	return 0;
}
