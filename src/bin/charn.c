#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
 
void onDisplay(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

int main(int argc,char **argv){
	xcb_randr_get_screen_info_cookie_t sict;
	xcb_randr_get_screen_info_reply_t *sirt;
	const xcb_setup_t *xcbsetup;
	xcb_generic_error_t *xcberr;
	xcb_connection_t *xcb;
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
	printf("Connected using XCB protocol %hu.%hu\n",
			xcbsetup->protocol_major_version,
			xcbsetup->protocol_minor_version);
	xwin = xcb_generate_id(xcb);
	sict = xcb_randr_get_screen_info(xcb,xwin);
	if((sirt = xcb_randr_get_screen_info_reply(xcb,sict,&xcberr)) == NULL){
		// FIXME use xcberr
		fprintf(stderr,"Coudln't get XRandR screen info\n");
		return EXIT_FAILURE;
	}
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

