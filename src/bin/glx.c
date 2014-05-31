#include <glx.h>
#include <stdio.h>
#include <string.h>
#include <GL/glx.h>
#include <GL/glut.h>

static int
get_glx_vendor(void){
	const GLubyte *vend,*rend,*ver;

	if((ver = glGetString(GL_VERSION)) == NULL){
		fprintf(stderr,"Couldn't get OpenGL version\n");
		return -1;
	}
	printf("Using OpenGL %s\n",ver);
	if((vend = glGetString(GL_VENDOR)) == NULL){
		fprintf(stderr,"Couldn't get OpenGL vendor\n");
		return -1;
	}
	if((rend = glGetString(GL_RENDERER)) == NULL){
		fprintf(stderr,"Couldn't get OpenGL renderer\n");
		return -1;
	}
	printf("GL server: %s %s\n",vend,rend);
	printf("Using GLUT %d\n",GLUT_API_VERSION);
	return 0;
}

int init_glx(Display *d,xcb_window_t window){
	GLXFBConfig *glfb,ourfb;
	int maj,min,numfbs,i;
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
	if((glfb = glXGetFBConfigs(d,0,&numfbs)) == NULL || numfbs <= 0){
		fprintf(stderr,"Couldn't detect GLX framebuffers\n");
		return -1;
	}
	printf("%d GLX framebuffer%s\n",numfbs,numfbs == 1 ? "" : "s");
	xvi = NULL;
	memset(&ourfb,0,sizeof(ourfb));
	// FIXME check all framebuffers; the first we find might not be correct
	for(i = 0 ; i < numfbs ; ++i){
		XVisualInfo *ixvi;
		ourfb = glfb[i];
		if( (ixvi = glXGetVisualFromFBConfig(d,ourfb)) ){
			if(xvi){
				XFree(xvi);
			}
			xvi = ixvi;
		//	r/g/b: 0x%lx/0x%lx/0x%lx colormap: %d bits: %d\n",
			printf("[GLXfb %02d] screen: %d depth: %d class: %d\n",
				i,xvi->screen,xvi->depth,xvi->class);
				//xvi->red_mask,xvi->green_mask,xvi->blue_mask,
				//xvi->colormap_size,xvi->bits_per_rgb);
		}
	}
	if(xvi == NULL){
		fprintf(stderr,"Couldn't find any XVisualInfo\n");
		return -1;
	}
	if((glctx = glXCreateContext(d,xvi,NULL,GL_TRUE)) == NULL){
		fprintf(stderr,"Couldn't create GLX context\n");
		XFree(xvi);
		return -1;
	}
	XFree(xvi);
	draw = glXCreateWindow(d,ourfb,window,NULL);
	if(!glXMakeCurrent(d,draw,glctx)){
		fprintf(stderr,"Couldn't activate GLX context\n");
		return -1;
	}
	if(get_glx_vendor()){
		return -1;
	}
	return 0;
}
