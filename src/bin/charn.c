#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <xcb/xcb_aux.h>
 
void onDisplay(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

static void
resizecb(int width,int height){
	printf("GLUT resize event (%dx%d)\n",width,height);
}

int main(int argc,char **argv){
	xcb_randr_get_screen_info_cookie_t sict;
	xcb_randr_get_screen_info_reply_t *sirt;
	xcb_randr_query_version_cookie_t rqvct;
	xcb_randr_query_version_reply_t *rqvrt;
	xcb_randr_screen_size_t *sizes,curgeom;
	int prefscr,cursize,numsizes;
	const xcb_setup_t *xcbsetup;
	xcb_generic_error_t *xcberr;
	xcb_connection_t *xcb;
	xcb_screen_t *xscr;
	int z;

	if((xcb = xcb_connect(NULL,&prefscr)) == NULL){
		fprintf(stderr,"Couldn't connect to $DISPLAY via XCB\n");
		return EXIT_FAILURE;
	}
	if((xcbsetup = xcb_get_setup(xcb)) == NULL){
		fprintf(stderr,"Couldn't get XCB setup\n");
		return EXIT_FAILURE;
	}
	printf("Connected using XCB protocol %hu.%hu\n",
			xcbsetup->protocol_major_version,
			xcbsetup->protocol_minor_version);
	if((xscr = xcb_aux_get_screen(xcb,prefscr)) == NULL){
		fprintf(stderr,"Couldn't get XCB screen info\n");
		return EXIT_FAILURE;
	}
	// FIXME from whence these constants? they work like maxima, but
	// choosing too high a value gets nonsense results...
	rqvct = xcb_randr_query_version(xcb,256,256);
	if((rqvrt = xcb_randr_query_version_reply(xcb,rqvct,&xcberr)) == NULL){
		fprintf(stderr,"Couldn't get XRandR version info\n");
		return EXIT_FAILURE;
	}
	printf("Connected using XRandR protocol %d.%d\n",
			rqvrt->major_version,rqvrt->minor_version);
	free(rqvrt);
	sict = xcb_randr_get_screen_info(xcb,xscr->root);
	if((sirt = xcb_randr_get_screen_info_reply(xcb,sict,&xcberr)) == NULL){
		// FIXME use xcberr
		fprintf(stderr,"Couldn't get XRandR screen info\n");
		return EXIT_FAILURE;
	}
	cursize = sirt->sizeID;
	numsizes = sirt->nSizes;
	if((sizes = xcb_randr_get_screen_info_sizes(sirt)) == NULL){
		fprintf(stderr,"Couldn't get XRandR size info\n");
		return EXIT_FAILURE;
	}
	for(z = 0 ; z < numsizes ; ++z){
		printf("Size %03d: %dx%d\n",z,sizes[z].width,sizes[z].height);
	}
	memcpy(&curgeom,sizes + cursize,sizeof(curgeom));
	free(sirt);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	printf("Screen size ID: %d/%d (%dx%d)\n",cursize,numsizes,curgeom.width,curgeom.height);
	glutInitWindowSize(curgeom.width,curgeom.height);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Charn");
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glutReshapeFunc(resizecb);
	glutDisplayFunc(onDisplay);
	glutMainLoop();
	xcb_disconnect(xcb);
	return EXIT_SUCCESS;
}

