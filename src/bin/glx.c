#include <glx.h>
#include <stdio.h>
#include <GL/glx.h>

static int
get_glx_vendor(void){
	const GLubyte *vend,*rend,*ver;

	if((vend = glGetString(GL_VENDOR)) == NULL){
		fprintf(stderr,"Couldn't get OpenGL vendor\n");
		return -1;
	}
	if((ver = glGetString(GL_VERSION)) == NULL){
		fprintf(stderr,"Couldn't get OpenGL version\n");
		return -1;
	}
	if((rend = glGetString(GL_RENDERER)) == NULL){
		fprintf(stderr,"Couldn't get OpenGL renderer\n");
		return -1;
	}
	printf("GL server: %s %s on %s\n",vend,ver,rend);
	return 0;
}

int init_glx(Display *d,xcb_window_t window){
	GLXFBConfig *glfb,ourfb;
	int maj,min,numfbs;
	GLXContext glctx;
	GLXDrawable draw;
	XVisualInfo *xvi;

	if(!glXQueryVersion(d,&maj,&min)){
		fprintf(stderr,"Couldn't query GLX version\n");
		return -1;
	}
	printf("Using GLX %d.%d\n",maj,min);
	if( (glctx = glXGetCurrentContext()) ){
		fprintf(stderr,"Already had a GLX context\n");
		return 0;
	}
	// FIXME 0 needs to be default screen
	if((glfb = glXGetFBConfigs(d,0,&numfbs)) == NULL || numfbs == 0){
		fprintf(stderr,"Couldn't detect GLX framebuffers\n");
		return -1;
	}
	// FIXME check all framebuffers, not just the zeroth
	ourfb = glfb[0];
	if((xvi = glXGetVisualFromFBConfig(d,ourfb)) == NULL){
		fprintf(stderr,"Couldn't extract XVisualInfo from GLX\n");
		return -1;
	}
	if((glctx = glXCreateContext(d,xvi,0,GL_TRUE)) == NULL){
		fprintf(stderr,"Couldn't create GLX context\n");
		return -1;
	}
	draw = glXCreateWindow(d,ourfb,window,0);
	if(!glXMakeCurrent(d,draw,glctx)){
		fprintf(stderr,"Couldn't activate GLX context\n");
		return -1;
	}
	if(get_glx_vendor()){
		return -1;
	}
	return 0;
}
