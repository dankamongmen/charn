#include <math.h>
#include <stdio.h>
#include <epoll.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <xcb/randr.h>
#include <xcb/xcb_aux.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_keysyms.h>

// FIXME wrap this state up into an object
static xcb_connection_t *xcbconn;

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

static int
xcb_poll(void){
	xcb_generic_event_t *xev;
	unsigned etype;

	if((xev = xcb_poll_for_event(xcbconn)) == NULL){
		fprintf(stderr,"Error polling for event\n");
		return -1;
	}
	etype = XCB_EVENT_RESPONSE_TYPE(xev);
	switch(etype){
		case XCB_KEY_PRESS:{
			xcb_key_press_event_t *ev = (xcb_key_press_event_t *)xev;
			xcb_key_symbols_t *syms;
			xcb_keysym_t sym;

			// FIXME all very experimental!
			if((syms = xcb_key_symbols_alloc(xcbconn)) == NULL){
				fprintf(stderr,"Couldn't allocate key symbols\n");
			}else if(!(sym = xcb_key_press_lookup_keysym(syms,ev,0))){
				fprintf(stderr,"Couldn't translate keycode %d\n",ev->detail);
			}else{
				printf("GOT KEYSYM?\n");
			}
			xcb_key_symbols_free(syms);
			break;
		}case XCB_KEY_RELEASE:{
			xcb_key_release_event_t *ev = (xcb_key_release_event_t *)xev;
			xcb_key_symbols_t *syms;
			xcb_keysym_t sym;

			// FIXME all very experimental!
			if((syms = xcb_key_symbols_alloc(xcbconn)) == NULL){
				fprintf(stderr,"Couldn't allocate key symbols\n");
			}else if(!(sym = xcb_key_release_lookup_keysym(syms,ev,0))){
				fprintf(stderr,"Couldn't translate keycode %d\n",ev->detail);
			}else{
				printf("GOT KEYSYM?\n");
			}
			xcb_key_symbols_free(syms);
			break;
		}case XCB_BUTTON_PRESS:
			fprintf(stderr,"XCB button press\n");
			break;
		case XCB_EXPOSE:
			fprintf(stderr,"XCB expose\n");
			break;
		case XCB_CLIENT_MESSAGE:
			fprintf(stderr,"XCB client\n");
			break;
		case XCB_MAPPING_NOTIFY:
			fprintf(stderr,"XCB mapping\n");
			break;
		case XCB_MOTION_NOTIFY:
			fprintf(stderr,"XCB motion\n");
			break;
		default:
			fprintf(stderr,"unhandled XCB event %d\n",etype);
			break;
	}
	free(xev);
	return 0;
}

static void
xcbcb(void){
	xcb_poll();
}

int xcb_init(Display *disp){
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

	if(disp){ // mixed X11+XCB mode
		if((xcb = XGetXCBConnection(disp)) == NULL){
			fprintf(stderr,"Couldn't extract XCB connection from X11\n");
			goto err;
		}
	}else if((xcb = xcb_connect(NULL,&prefscr)) == NULL){
		fprintf(stderr,"Couldn't connect to $DISPLAY via XCB\n");
		goto err;
	}
	if((xcbsetup = xcb_get_setup(xcb)) == NULL){
		fprintf(stderr,"Couldn't get XCB setup\n");
		goto err;
	}
	if(get_xcb_vendor(xcbsetup)){
		goto err;
	}
	xcbfd = xcb_get_file_descriptor(xcb);
	printf("Connected using XCB protocol %hu.%hu on fd %d\n",
			xcbsetup->protocol_major_version,
			xcbsetup->protocol_minor_version,xcbfd);
	if((xscr = xcb_aux_get_screen(xcb,prefscr)) == NULL){
		fprintf(stderr,"Couldn't get XCB screen info\n");
		goto err;
	}
	// FIXME from whence these constants? they work like maxima, but
	// choosing too high a value gets nonsense results...
	rqvct = xcb_randr_query_version(xcb,256,256);
	if((rqvrt = xcb_randr_query_version_reply(xcb,rqvct,&xcberr)) == NULL){
		fprintf(stderr,"Couldn't get XCB-XRandR version info\n");
		goto err;
	}
	free(rqvrt);
	screenit = xcb_setup_roots_iterator(xcbsetup);
	scrcount = screenit.rem;
	printf("Connected using XCB-XRandR protocol %d.%d (%d screen%s)\n",
			rqvrt->major_version,rqvrt->minor_version
			,screenit.rem,screenit.rem == 1 ? "" : "s");
	for(z = 0 ; z < scrcount ; ++z){
		xcb_void_cookie_t cwin,cmap;
		xcb_generic_error_t *xerr;
		uint32_t values[2];
		xcb_window_t wid;
		uint32_t mask;
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
			fprintf(stderr,"Couldn't get XCB-XRandR screen info\n");
			goto err;
		}
		cursize = sirt->sizeID;
		numsizes = sirt->nSizes;
		if((sizes = xcb_randr_get_screen_info_sizes(sirt)) == NULL){
			fprintf(stderr,"Couldn't get XCB-XRandR size info\n");
			free(sirt);
			goto err;
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
		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[0] = screenit.data->white_pixel;
		values[1] = XCB_EVENT_MASK_EXPOSURE;
		wid = xcb_generate_id(xcb);
		cwin = xcb_create_window_checked(xcb,
				XCB_COPY_FROM_PARENT,
				wid,
				screenit.data->root,
				0, 0,
				curgeom.width, curgeom.height,
				0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT,
				screenit.data->root_visual,
				mask, values);
		cmap = xcb_map_window_checked(xcb,wid);
		if((xerr = xcb_request_check(xcb,cwin)) || (xerr = xcb_request_check(xcb,cmap))){
			fprintf(stderr,"Error mapping root window (%d)\n",xerr->error_code);
			goto err;
		}
		values[0] = XCB_STACK_MODE_ABOVE;
		xcb_configure_window(xcb, wid, XCB_CONFIG_WINDOW_STACK_MODE, values);
		//xcb_set_input_focus(xcb,XCB_INPUT_FOCUS_PARENT,wid,XCB_CURRENT_TIME);
		xcb_screen_next(&screenit);
	}
	xcb_grab_server(xcb);
	/*xcb_grab_keyboard(xcb,1,screenit.data->root,XCB_CURRENT_TIME,
			XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);*/
	if(add_event_fd(xcbfd,xcbcb)){
		goto err;
	}
	xcbconn = xcb;
	return 0;

err:
	xcb_disconnect(xcb);
	return -1;
}
