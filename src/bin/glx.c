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
	int maj, min, numfbs, i, fbxid;
	GLXFBConfig *glfb, ourfb;
	XVisualInfo xvi, *xviptr;
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
	printf("%d GLX framebuffer%s\n",numfbs,numfbs == 1 ? "" : "s");
	xviptr = NULL;
	memset(&xvi, 0, sizeof(xvi));
	memset(&ourfb, 0, sizeof(ourfb));
	for(i = 0 ; i < numfbs ; ++i){
		XVisualInfo *ixvi;
		if( (ixvi = glXGetVisualFromFBConfig(d, glfb[i])) ){
			int samples, dbuffered, xid;
			if(xviptr){
				XFree(xviptr);
			}
			xviptr = ixvi;
			glXGetFBConfigAttrib(d, glfb[i], GLX_FBCONFIG_ID, &xid);
			glXGetFBConfigAttrib(d, glfb[i], GLX_SAMPLES, &samples);
			glXGetFBConfigAttrib(d, glfb[i], GLX_DOUBLEBUFFER, &dbuffered);
			//r/g/b: 0x%lx/0x%lx/0x%lx colormap: %d bits: %d\n",
			//xviptr->red_mask,xviptr->green_mask,xviptr->blue_mask,
			//xviptr->colormap_size,xviptr->bits_per_rgb);
			printf("[fb 0x%03x] scrn %d dpp %d class %d samples %2d dbuf? %d vID %lu\n",
				xid, xviptr->screen, xviptr->depth,
				xviptr->class, samples, dbuffered,
				xviptr->visualid);
			// FIXME intelligently select framebuffer config...how?
			// first framebuffer always seems to work, others don't.
			if(i == 0) {
				ourfb = glfb[i];
				memcpy(&xvi, ixvi, sizeof(xvi));
				fbxid = xid;
			}
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
	printf("Using framebuffer XID 0x%03x\n", fbxid);
	if((glctx = glXCreateContext(d,&xvi,NULL,GL_TRUE)) == NULL){
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
	return 0;
}
