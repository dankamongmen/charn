#include <xcb.h>
#include <epoll.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>

static void
xcbcb(void){
	xcb_poll();
}

int event_loop(int xcbfd){
	struct epoll_event xcbev;
	int eqfd,e,err;
	sigset_t ss;

	sigemptyset(&ss);
	if((eqfd = epoll_create1(EPOLL_CLOEXEC)) <= 0){
		fprintf(stderr,"Couldn't get epoll fd (%s)\n",strerror(errno));
		return -1;
	}
	xcbev.events = EPOLLIN | EPOLLRDHUP | EPOLLPRI | EPOLLET;
	xcbev.data.ptr = xcbcb;
	if(epoll_ctl(eqfd,EPOLL_CTL_ADD,xcbfd,&xcbev)){
		fprintf(stderr,"Couldn't set up XCB for epoll (%s)\n",strerror(errno));
		close(eqfd);
		return -1;
	}
	fprintf(stderr,"Entering main event loop (Linux epoll, fd %d)...\n",eqfd);
	do{
		struct epoll_event events[10];

		while((e = epoll_pwait(eqfd,events,sizeof(events) / sizeof(*events),-1,&ss)) >= 0){
			while(e--){
				((void(*)(void))events[e].data.ptr)();
			}
		}
		if((err = errno) != EINTR){
			fprintf(stderr,"Error polling for events (%s)\n",strerror(errno));
		}else{
			fprintf(stderr,"Poll interrupted by signal\n");
		}
	}while(err == EINTR);
	if(close(eqfd)){
		fprintf(stderr,"Error closing epoll fd (%s)\n",strerror(errno));
		return -1;
	}
	return 0;
}
