#include "charn.h"
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

static int
print_fbvisual(Display *d, const GLXFBConfig *fb, const XVisualInfo *xvi){
	int samples, dbuffered, xid;

	glXGetFBConfigAttrib(d, *fb, GLX_FBCONFIG_ID, &xid);
	glXGetFBConfigAttrib(d, *fb, GLX_SAMPLES, &samples);
	glXGetFBConfigAttrib(d, *fb, GLX_DOUBLEBUFFER, &dbuffered);
	//r/g/b: 0x%lx/0x%lx/0x%lx colormap: %d bits: %d\n",
	//xvi->red_mask,xvi->green_mask,xvi->blue_mask,
	//xvi->colormap_size,xvi->bits_per_rgb);
	return printf("fb 0x%03x scrn %d dpp %d class %d samples %2d dbuf? %d vID %lu\n",
		xid, xvi->screen, xvi->depth, xvi->class, samples, dbuffered,
		xvi->visualid);
}

int init_glx(Display *d,xcb_window_t window){
	GLXFBConfig *glfb, ourfb;
	XVisualInfo xvi, *xviptr;
	int maj, min, numfbs, i;
	GLXContext glctx;
	GLXDrawable draw;

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
	// glxinfo decodes this information under the GLXFBConfigs header.
	printf("%d possible GLX framebuffer%s\n",numfbs,numfbs == 1 ? "" : "s");
	xviptr = NULL;
	memset(&xvi, 0, sizeof(xvi));
	memset(&ourfb, 0, sizeof(ourfb));
	for(i = 0 ; i < numfbs ; ++i){
		XVisualInfo *ixvi;
		if( (ixvi = glXGetVisualFromFBConfig(d, glfb[i])) ){
			if(xviptr){
				XFree(xviptr);
			}
			if(Verbose){
				print_fbvisual(d, &glfb[i], ixvi);
			}
			// FIXME intelligently select framebuffer config...how?
			// first framebuffer always seems to work, others don't.
			if(!xviptr){
				ourfb = glfb[i];
				memcpy(&xvi, ixvi, sizeof(xvi));
			}
			xviptr = ixvi;
		}else{
			// FIXME what are these? glxinfo treats them as real
			// framebuffer types with depths of 0. there's a lot
			// of them, so don't clutter stderr for now...
			// fprintf(stderr,"Couldn't get visual info for framebuffer %d\n", i);
		}
	}
	if(xviptr == NULL){
		fprintf(stderr,"Couldn't find any XVisualInfo\n");
		return -1;
	}
	XFree(xviptr);
	printf("Selected ");
	print_fbvisual(d, &ourfb, &xvi);
	if((glctx = glXCreateContext(d, &xvi, NULL, GL_TRUE)) == NULL){
		fprintf(stderr,"Couldn't create GLX context\n");
		return -1;
	}
	if((draw = glXCreateWindow(d,ourfb,window,NULL)) == 0){
		fprintf(stderr,"Couldn't create GLX window\n");
		return -1;
	}
	if(glXMakeCurrent(d,draw,glctx) != GL_TRUE){
		fprintf(stderr,"Couldn't activate GLX context\n");
		return -1;
	}
	if(get_glx_vendor()){
		return -1;
	}
	printf("GLX successfully initialized\n");
	return 0;
}
