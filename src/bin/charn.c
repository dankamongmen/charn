#include <stdio.h>
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

int main(int argc,char **argv){
	xcb_randr_get_screen_info_cookie_t sict;
	xcb_randr_get_screen_info_reply_t *sirt;
	xcb_randr_query_version_cookie_t rqvct;
	xcb_randr_query_version_reply_t *rqvrt;
	const xcb_setup_t *xcbsetup;
	xcb_generic_error_t *xcberr;
	xcb_connection_t *xcb;
	xcb_screen_t *xscr;
	xcb_window_t xwin;
	int prefscr;

	if((xcb = xcb_connect(NULL,&prefscr)) == NULL){
		fprintf(stderr,"Couldn't connect to $DISPLAY via XCB\n");
		return EXIT_FAILURE;
	}
	if((xcbsetup = xcb_get_setup(xcb)) == NULL){
		fprintf(stderr,"Couldn't get XCB setup\n");
		return EXIT_FAILURE;
	}
	xwin = xcb_generate_id(xcb) ;
	printf("Connected using XCB protocol %hu.%hu ID %ju\n",
			xcbsetup->protocol_major_version,
			xcbsetup->protocol_minor_version,
			(uintmax_t)xwin);
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
	sict = xcb_randr_get_screen_info(xcb,xwin);
	if((sirt = xcb_randr_get_screen_info_reply(xcb,sict,&xcberr)) == NULL){
		// FIXME use xcberr
		fprintf(stderr,"Couldn't get XRandR screen info\n");
		return EXIT_FAILURE;
	}
	free(sirt);
	/* Glut-related initialising functions */
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Charn");
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	/* We can display it if everything goes OK */
	glutDisplayFunc(onDisplay);
	glutMainLoop();
	xcb_disconnect(xcb);
	return EXIT_SUCCESS;
}

