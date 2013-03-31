#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <xcb/xcb_aux.h>

int xcb_init(void){
	xcb_randr_get_screen_info_cookie_t sict;
	xcb_randr_get_screen_info_reply_t *sirt;
	xcb_randr_query_version_cookie_t rqvct;
	xcb_randr_query_version_reply_t *rqvrt;
	xcb_randr_screen_size_t *sizes,curgeom;
	xcb_screen_iterator_t screenit;
	int prefscr,cursize,numsizes;
	const xcb_setup_t *xcbsetup;
	xcb_generic_error_t *xcberr;
	xcb_connection_t *xcb;
	xcb_screen_t *xscr;
	int z,scrcount;

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
	free(rqvrt);
	screenit = xcb_setup_roots_iterator(xcbsetup);
	scrcount = screenit.rem;
	printf("Connected using XRandR protocol %d.%d (%d screen%s)\n",
			rqvrt->major_version,rqvrt->minor_version
			,screenit.rem,screenit.rem == 1 ? "" : "s");
	for(z = 0 ; z < scrcount ; ++z){
		sict = xcb_randr_get_screen_info(xcb,screenit.data->root);
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
		if(z % 4 != 2){
			printf("\n");
		}
		memcpy(&curgeom,sizes + cursize,sizeof(curgeom));
		free(sirt);
		xcb_screen_next(&screenit);
	}
	return 0;
}
