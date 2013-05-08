#include <xcb.h>
#include <math.h>
#include <stdio.h>
#include <epoll.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <xcb/randr.h>
#include <xcb/xcb_aux.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_keysyms.h>

// FIXME wrap this state up into an object
static xcb_key_symbols_t *syms;
static xcb_connection_t *xcbconn;
static xcb_ewmh_connection_t ewmhconn;

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
	printf("X server: %s %u.%u.%u.%u\n",vend,
			xcb->release_number / 10000000u,
			(xcb->release_number / 100000u) % 100u,
			(xcb->release_number / 1000u) % 100u,
			xcb->release_number % 1000u);
	free(vend);
	return 0;
}

static int
xcb_poll(void){
	xcb_generic_event_t *xev;
	unsigned etype;

	if((xev = xcb_poll_for_event(xcbconn)) == NULL){
		if(!xcb_connection_has_error(xcbconn)){
			fprintf(stderr,"No XCB event was available\n");
			return 0;
		}
		fprintf(stderr,"Connection to X server was lost\n");
		return -1;
	}
	etype = XCB_EVENT_RESPONSE_TYPE(xev);
	switch(etype){
		case XCB_KEY_PRESS:{
			xcb_key_press_event_t *ev = (xcb_key_press_event_t *)xev;
			xcb_keysym_t sym;

			if(!(sym = xcb_key_press_lookup_keysym(syms,ev,0))){
				fprintf(stderr,"Couldn't translate keycode %d\n",ev->detail);
			}else{
				printf("GOT KEYPRESS?\n");
			}
			printf("FREED KEYSYM!\n");
			break;
		}case XCB_KEY_RELEASE:{
			xcb_key_release_event_t *ev = (xcb_key_release_event_t *)xev;
			xcb_keysym_t sym;

			if(!(sym = xcb_key_release_lookup_keysym(syms,ev,0))){
				fprintf(stderr,"Couldn't translate keycode %d\n",ev->detail);
			}else{
				printf("GOT KEYRELEASE?\n");
			}
			printf("FREED KEYSYM!\n");
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

static int
xcbcb(void){
	return xcb_poll();
}

int xcb_stop(void){
	if(syms){
		xcb_key_symbols_free(syms);
		syms = NULL;
	}
	return 0;
}

xcb_window_t xcb_init(Display *disp){
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
	xcb_window_t wid;

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
	if(xcb_ewmh_init_atoms(xcb,&ewmhconn) == NULL){
		fprintf(stderr,"Couldn't get EWMH properties\n");
		goto err;
	}
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
	if((syms = xcb_key_symbols_alloc(xcb)) == NULL){
		fprintf(stderr,"Couldn't allocate key symbols\n");
		goto err;
	}
	wid = (xcb_window_t)-1;
	for(z = 0 ; z < scrcount ; ++z){
		xcb_void_cookie_t cwin,cmap;
		xcb_generic_error_t *xerr;
		float diag,inchw,inchh;
		uint32_t values[2];
		uint32_t mask;

		sict = xcb_randr_get_screen_info(xcb,screenit.data->root);
		diag = sqrt((unsigned long)screenit.data->width_in_millimeters * screenit.data->width_in_millimeters +
			(unsigned long)screenit.data->height_in_millimeters * screenit.data->height_in_millimeters);
		inchw = screenit.data->width_in_millimeters * 0.0394;
		inchh = screenit.data->height_in_millimeters * 0.0394;
		printf("Screen %d %hux%humm (%.2f ratio), %.0fmm diag (%.2fx%.2fin, %.2fin diag)\n",
				screenit.index,
				screenit.data->width_in_millimeters,
				screenit.data->height_in_millimeters,
				(double)screenit.data->width_in_millimeters / 
				screenit.data->height_in_millimeters,
				diag,
				inchw,inchh,diag * 0.0394);
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
			printf("[%02d] %4dx%-4d  ",z + 1,sizes[z].width,sizes[z].height);
			if(z % 4 == 3){
				printf("\n");
			}
		}
		if(z % 4){
			printf("\n");
		}
		memcpy(&curgeom,sizes + cursize,sizeof(curgeom));
		printf("Using geometry %d/%d (%dx%d) (%.0fx%.0f DPI, %.02f ratio)\n",
				cursize + 1,numsizes,
				curgeom.width,curgeom.height,
				round(curgeom.width / inchw),
				round(curgeom.height / inchh),
				(double)curgeom.width / curgeom.height);
		free(sirt);
		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[0] = screenit.data->white_pixel;
		values[1] = XCB_EVENT_MASK_KEY_RELEASE |
			XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_POINTER_MOTION;
		// FIXME for multiple screens do we want multiple xcb ids?
		if((wid = xcb_generate_id(xcb)) == (xcb_window_t)-1){
			fprintf(stderr,"Couldn't generate XCB ID\n");
			return -1;
		}
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
		xcbconn = xcb;
		values[0] = XCB_STACK_MODE_ABOVE;
		xcb_configure_window(xcb, wid, XCB_CONFIG_WINDOW_STACK_MODE, values);
		//xcb_set_input_focus(xcb,XCB_INPUT_FOCUS_PARENT,wid,XCB_CURRENT_TIME);
		xcb_screen_next(&screenit);
	}
	/*xcb_grab_server(xcb);
	xcb_grab_keyboard(xcb,1,screenit.data->root,XCB_CURRENT_TIME,
			XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);*/
	if(add_event_fd(xcbfd,xcbcb)){
		goto err;
	}
	xcbconn = xcb;
	return wid;

err:
	xcb_disconnect(xcb);
	return -1;
}

int set_title(xcb_window_t wid,const char *title){
	/*
	xcb_void_cookie_t cookie;
	xcb_generic_error_t *err;
	*/

	xcb_ewmh_set_wm_name(&ewmhconn,wid,strlen(title),title);
	/*
	cookie = xcb_change_property_checked(xcbconn,XCB_PROP_MODE_REPLACE,wid,
			XCB_ATOM_WM_NAME,XCB_ATOM_STRING,sizeof(title),
			strlen(title),title);
	if( (err = xcb_request_check(xcbconn,cookie)) ){
		fprintf(stderr,"Couldn't set window title to %s\n",title);
		free(err);
		return -1;
	}
	*/
	printf("Set window title to %s\n",title);
	return 0;
}
