#ifndef CHARN_EPOLL
#define CHARN_EPOLL

int event_init(void);
int event_loop(void);
int add_event_fd(int,void (*)(void));

#endif
