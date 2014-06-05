#include <dbus.h>
#include <epoll.h>
#include <stdio.h>
#include <dbus/dbus.h>

typedef struct dbus_state {
	DBusConnection *session,*system;
	int sessionfd,systemfd;
} dbus_state;

static dbus_state ctx;

typedef int (*dbuscb)(void);

static int
handle_dbus_msg(DBusConnection *dbc,const char *name){
	DBusDispatchStatus dstat;

	fprintf(stderr,"%s DBus callback [status %d]\n",name,
			dbus_connection_get_dispatch_status(dbc));
	dstat = dbus_connection_dispatch(dbc);
	if(dstat != DBUS_DISPATCH_COMPLETE){ // FIXME do better
		fprintf(stderr,"Couldn't empty the DBus dispatch queue\n");
	}
	return 0;
}

static int
sessioncb(void){
	return handle_dbus_msg(ctx.session,"Session");
}

static int
systemcb(void){
	return handle_dbus_msg(ctx.system,"System");
}

static DBusConnection *
dbus_connect(DBusBusType bustype,const char *name,dbuscb cb,int *fd){
	DBusConnection *c;
	DBusError err;

	dbus_error_init(&err);
	if((c = dbus_bus_get(bustype,&err)) == NULL){
		return NULL;
	}
	if(dbus_error_is_set(&err)){
		fprintf(stderr,"Connecting to %s D-Bus: %s\n",name,err.message);
		dbus_error_free(&err);
		return NULL;
	}
	dbus_connection_set_exit_on_disconnect(c,0); // seriously?!?
	if(dbus_connection_get_unix_fd(c,fd) == 0){
		fprintf(stderr,"Extracting %s D-Bus fd: %s\n",name,err.message);
		dbus_error_free(&err); // FIXME disconnect?
		return NULL;
	}
	printf("DBus[%s] at fd %d%s\n",name,*fd,dbus_bus_get_unique_name(c));
	dbus_error_free(&err);
	if(add_event_fd(*fd,cb)){ // FIXME disconnect?
		return NULL;
	}
	return c;
}	

int dbus_init(void){
	if((ctx.session = dbus_connect(DBUS_BUS_SESSION,"session",sessioncb,&ctx.sessionfd)) == NULL){
		return -1;
	}
	if((ctx.system = dbus_connect(DBUS_BUS_SYSTEM,"system",systemcb,&ctx.systemfd)) == NULL){
		return -1;
	}
	return 0;
}
