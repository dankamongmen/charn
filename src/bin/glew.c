#include <glew.h>
#include <stdio.h>
#include <GL/glew.h>

int glew_init(void){
	GLenum err;
       
	if((err	= glewInit()) != GLEW_OK){
		fprintf(stderr,"Error initializing GLEW: %s\n",glewGetErrorString(err));
		return -1;
	}
	printf("Status: Using GLEW %s\n",glewGetString(GLEW_VERSION));
	return 0;
}
