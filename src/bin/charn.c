#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <SOIL/SOIL.h>
#include <xcb/xcb_aux.h>

static GLuint texid; // texture id of quad
 
static void
onDisplay(void){
	//glClear(GL_COLOR_BUFFER_BIT);
	if(texid){
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
		glBindTexture(GL_TEXTURE_2D,texid);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0.0,1.0);
	glVertex3f(-1.0f,-1.0f,0.0f);
	glTexCoord2f(0.0,0.0);
	glVertex3f(-1.0f,1.0f,0.0f);
	glTexCoord2f(1.0,0.0);
	glVertex3f(1.0f,1.0f,0.0f);
	glTexCoord2f(1.0,1.0);
	glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
	if(texid){
		glDisable(GL_TEXTURE_2D);
	}
	glutSwapBuffers();
}

static void
resizecb(int width,int height){
	printf("GLUT resize event (%dx%d)\n",width,height);
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

static int
gl_init(void){
	GLenum types[] = {
		GL_VENDOR,
		GL_RENDERER,
		GL_VERSION,
		//GL_EXTENSIONS
	},t;

	printf("OpenGL ");
	for(t = 0 ; t < sizeof(types) / sizeof(*types) ; ++t){
		const GLubyte *glver = glGetString(types[t]);

		if(glver){
			printf("%s ",glver);
		}else{
			return -1;
		}
	}
	printf("\n");
	return 0;
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
		printf("[%02d] %4dx%-4d  ",z,sizes[z].width,sizes[z].height);
		if(z % 4 == 3){
			printf("\n");
		}
	}
	if(z % 4 != 3){
		printf("\n");
	}
	memcpy(&curgeom,sizes + cursize,sizeof(curgeom));
	free(sirt);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	printf("Screen size ID: %d/%d (%dx%d)\n",cursize,numsizes,curgeom.width,curgeom.height);
	glutInitWindowSize(curgeom.width / 2,curgeom.height / 2);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Charn");
	if(gl_init()){
		fprintf(stderr,"Error getting OpenGL version info\n");
		return EXIT_FAILURE;
	}
	if(argv[1]){
		texid = SOIL_load_OGL_texture(argv[1],0,0,0);
		if(texid == 0){
			fprintf(stderr,"Failure loading texture from %s (%s?)\n",
					argv[1],SOIL_last_result());
			return EXIT_FAILURE;
		}
	}else{
		texid = 0;
	}
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glutReshapeFunc(resizecb);
	glutDisplayFunc(onDisplay);
	glutMainLoop();
	xcb_disconnect(xcb);
	return EXIT_SUCCESS;
}

