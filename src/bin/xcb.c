#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <xcb/xcb_aux.h>

static int
get_xcb_vendor(const xcb_setup_t *xcb){
	char *vend;
	int len;

	if((len = xcb_setup_vendor_length(xcb)) <= 0){
		fprintf(stderr,"Invalid vendor length %d\n",len);
		return -1;
	}
	if((vend = malloc(sizeof(*vend) * (1 + len))) == NULL){
		fprintf(stderr,"Couldn't allocate %db for vendor string\n",len + 1);
		return -1;
	}
	memcpy(vend,xcb_setup_vendor(xcb),len);
	vend[len] = '\0';
	printf("X server vendor: %s\n",vend);
	free(vend);
	return 0;
}

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
	int z,scrcount,xcbfd;
	xcb_screen_t *xscr;

	if((xcb = xcb_connect(NULL,&prefscr)) == NULL){
		fprintf(stderr,"Couldn't connect to $DISPLAY via XCB\n");
		return -1;
	}
	if((xcbsetup = xcb_get_setup(xcb)) == NULL){
		fprintf(stderr,"Couldn't get XCB setup\n");
		return -1;
	}
	if(get_xcb_vendor(xcbsetup)){
		return -1;
	}
	xcbfd = xcb_get_file_descriptor(xcb);
	printf("Connected using XCB protocol %hu.%hu on fd %d\n",
			xcbsetup->protocol_major_version,
			xcbsetup->protocol_minor_version,xcbfd);
	if((xscr = xcb_aux_get_screen(xcb,prefscr)) == NULL){
		fprintf(stderr,"Couldn't get XCB screen info\n");
		return -1;
	}
	// FIXME from whence these constants? they work like maxima, but
	// choosing too high a value gets nonsense results...
	rqvct = xcb_randr_query_version(xcb,256,256);
	if((rqvrt = xcb_randr_query_version_reply(xcb,rqvct,&xcberr)) == NULL){
		fprintf(stderr,"Couldn't get XRandR version info\n");
		return -1;
	}
	free(rqvrt);
	screenit = xcb_setup_roots_iterator(xcbsetup);
	scrcount = screenit.rem;
	printf("Connected using XRandR protocol %d.%d (%d screen%s)\n",
			rqvrt->major_version,rqvrt->minor_version
			,screenit.rem,screenit.rem == 1 ? "" : "s");
	for(z = 0 ; z < scrcount ; ++z){
		float diag;

		sict = xcb_randr_get_screen_info(xcb,screenit.data->root);
		diag = sqrt((unsigned long)screenit.data->width_in_millimeters * screenit.data->width_in_millimeters +
			(unsigned long)screenit.data->height_in_millimeters * screenit.data->height_in_millimeters);
		printf("Screen %d %hux%humm, %.2fmm diag (%.2fx%.2fin, %.2fin diag)\n",screenit.index,
				screenit.data->width_in_millimeters,
				screenit.data->height_in_millimeters, diag,
				screenit.data->width_in_millimeters * 0.0394,
				screenit.data->height_in_millimeters * 0.0394, diag * 0.0394);
		if((sirt = xcb_randr_get_screen_info_reply(xcb,sict,&xcberr)) == NULL){
			// FIXME use xcberr
			fprintf(stderr,"Couldn't get XRandR screen info\n");
			return -1;
		}
		cursize = sirt->sizeID;
		numsizes = sirt->nSizes;
		if((sizes = xcb_randr_get_screen_info_sizes(sirt)) == NULL){
			fprintf(stderr,"Couldn't get XRandR size info\n");
			return -1;
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
		printf("Screen size ID: %02d/%02d (%dx%d)\n",cursize,numsizes - 1,
				curgeom.width,curgeom.height);
		free(sirt);
		xcb_screen_next(&screenit);
	}
	return xcbfd;
}
