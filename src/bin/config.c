#include "charn.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

int init_config(CharnConfig *cc){
	memset(cc, 0, sizeof(*cc));
	return 0;
}

int load_config(CharnConfig *cc __attribute__ ((unused)), const char *dir, const char *file){
	char *path = NULL;
	if(dir){
		path = malloc(strlen(dir) + strlen(file) + 2);
		strcpy(path, dir);
		strcat(path, "/");
		strcat(path, file);
	}else{
		path = strdup(file);
	}
	Vfprintf(stdout, "Looking for config file at %s\n", path);
	// FIXME load config into cc
	free(path);
	return 0;
}
