#include <xcb.h>
#include <epoll.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>

// FIXME make a real singleton
static int eqfd = -1;

int add_event_fd(int fd,int (*cb)(void)){
	struct epoll_event ev;

	// FIXME move to edge-triggered events once basic work is done
	ev.events = EPOLLIN | EPOLLRDHUP | EPOLLPRI/* | EPOLLET*/;
	ev.data.ptr = cb;
	if(epoll_ctl(eqfd,EPOLL_CTL_ADD,fd,&ev)){
		fprintf(stderr,"Couldn't set up epoll for %d (%s)\n",fd,strerror(errno));
		return -1;
	}
	return 0;
}

int event_init(void){
	if((eqfd = epoll_create1(EPOLL_CLOEXEC)) <= 0){
		fprintf(stderr,"Couldn't get epoll fd (%s)\n",strerror(errno));
		return -1;
	}
	return 0;
}

int event_loop(void){
	sigset_t ss;
	int e,err;

	sigemptyset(&ss);
	fprintf(stderr,"Entering main event loop (Linux epoll, fd %d)...\n",eqfd);
	do{
		struct epoll_event events[10];

		while((e = epoll_pwait(eqfd,events,sizeof(events) / sizeof(*events),-1,&ss)) >= 0){
			printf("*** %d events ***\n",e);
			while(e--){
				if(((int(*)(void))events[e].data.ptr)()){
					goto cberror;
				}
			}
		}
		if((err = errno) != EINTR){
			fprintf(stderr,"Error polling for events (%s)\n",strerror(errno));
		}else{
			fprintf(stderr,"Poll interrupted by signal\n");
		}
	}while(err == EINTR);
cberror:
	if(close(eqfd)){
		fprintf(stderr,"Error closing epoll fd (%s)\n",strerror(errno));
		return -1;
	}
	eqfd = -1;
	return 0;
}
