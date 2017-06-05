/**
 * author: wangbin (gkwangbin@126.com)
 * date: 2017-05-18
 */
#ifndef FUNC_H
#define FUNC_H

#include <sys/socket.h>  
#include <sys/epoll.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <errno.h> 
#include <string.h>
#include <iostream>

#define MAX_EVENTS 	500 
#define S_PORT   	8090
#define TIME_OUT 	60

using namespace std;

struct myevent_s  
{  
    int fd;  
    void (*call_back)(int fd, int events, void *arg);  
    int events;  
    void *arg;  
    int status; // 1: in epoll wait list, 0 not in  
    char buff[128]; // recv data buffer  
    int len;  
    long last_active; // last active time  
};

/*
void EventSet(myevent_s *ev, int fd, void (*call_back)(int, int, void*), void *arg);
void EventAdd(int epollFd, int events, myevent_s *ev);
void EventDel(int epollFd, myevent_s *ev);
void AcceptConn(int fd, int events, void *arg);
void RecvData(int fd, int events, void *arg);
void SendData(int fd, int events, void *arg);
void InitListenSocket(int epollFd, short port);
*/

bool Exec();

#endif