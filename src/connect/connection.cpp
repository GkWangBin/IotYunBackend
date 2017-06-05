/**
 * author: wangbin (gkwangbin@126.com)
 * date: 2017-05-18
 */
#include "connection.h"
#include "log.h"
#include "logic.h"

using namespace std;  

/**
 *   typedef union epoll_data {
        void *ptr;
         int fd;
         __uint32_t u32;
         __uint64_t u64;
     } epoll_data_t;//保存触发事件的某个文件描述符相关的数据
     struct epoll_event {
         __uint32_t events;      // epoll event 
         epoll_data_t data;      // User data variable
     };
 */
 
/**
 * [EventSet description]
 * @param epollFd [description]
 * @param events  [description]
 * @param ev      [description]
 */
void EventSet(myevent_s *ev, int fd, void (*call_back)(int, int, void*), void *arg)  
{  
    ev->fd = fd;  
    ev->call_back = call_back;  
    ev->events = 0;  
    ev->arg = arg;
    ev->status = 0;
    ev->last_active = time(NULL);
}  

int g_epollFd;
myevent_s g_Events[MAX_EVENTS+1]; // g_Events[MAX_EVENTS] is used by listen fd

/**
 * [EventAdd description]
 * @param epollFd [description]
 * @param events  [description]
 * @param ev      [description]
 */
void EventAdd(int epollFd, int events, myevent_s *ev)  
{  
    struct epoll_event epv = {0, {0}};  
    int op;  
    epv.data.ptr = ev;  
    epv.events = ev->events = events;  
    /**
     epoll_ctl函数对epoll进行op类型的操作，op选项为
              EPOLL_CTL_ADD，对fd描述符注册event事件
              EPOLL_CTL_MOD，对fd描述符的event事件进行修改
              EPOLL_CTL_DEL，删除已注册的event事件
     */
    if(ev->status == 1){  
        op = EPOLL_CTL_MOD;
    }  
    else{  
        op = EPOLL_CTL_ADD;  
        ev->status = 1;  
    }
    if(epoll_ctl(epollFd, op, ev->fd, &epv) < 0){
        LOG_WARN("%s:%d(%s), Event Add failed, fd:%d", __FILE__, __LINE__, __FUNCTION__, ev->fd);
    }  
    else{
        //LOG_INFO("%s:%d(%s), Event Add OK,fd: %d", __FILE__, __LINE__, __FUNCTION__, ev->fd);
    }
}

// delete an event from epoll
/**
 * [EventDel description]
 * @param epollFd [description]
 * @param ev      [description]
 */
void EventDel(int epollFd, myevent_s *ev)  
{  
    struct epoll_event epv = {0, {0}};  
    if(ev->status != 1) 
    {    
        return; 
    } 
    epv.data.ptr = ev;  
    ev->status = 0;  
    epoll_ctl(epollFd, EPOLL_CTL_DEL, ev->fd, &epv);  
}  

  
void RecvData(int fd, int events, void *arg);  
void SendData(int fd, int events, void *arg);  

// accept new connections from clients
/**
 * [AcceptConn description]
 * @param fd     [description]
 * @param events [description]
 * @param arg    [description]
 */
void AcceptConn(int fd, int events, void *arg)  
{  
    struct sockaddr_in sin;  
    socklen_t len = sizeof(struct sockaddr_in);  
    int nfd, i;  
    // accept  
    if((nfd = accept(fd, (struct sockaddr*)&sin, &len)) == -1)  
    {  
        if(errno != EAGAIN && errno != EINTR)  
        {  
            LOG_WARN("%s:%d(%s), %s: bad accept", __FILE__, __LINE__, __FUNCTION__, __func__);
        }  
        return;  
    }  
    do  
    {  
        for(i = 0; i < MAX_EVENTS; i++)  
        {  
            if(g_Events[i].status == 0)  
            {  
                break;
            }
        }
        if(i == MAX_EVENTS)
        {
            LOG_WARN("%s:%d(%s), %s:max connection limit %d .", __FILE__, __LINE__, 
                __FUNCTION__, __func__, MAX_EVENTS);
            break;  
        }
        // set nonblocking  
        if(fcntl(nfd, F_SETFL, O_NONBLOCK) < 0) 
        {    
            break;
        }  
        // add a read event for receive data  
        EventSet(&g_Events[i], nfd, RecvData, &g_Events[i]); 
        EventAdd(g_epollFd, EPOLLIN|EPOLLET, &g_Events[i]);

        LOG_INFO("%s:%d(%s), new conn %s:%d,time:%d",__FILE__, __LINE__, __FUNCTION__, 
            inet_ntoa(sin.sin_addr), ntohs(sin.sin_port), (int)g_Events[i].last_active);  
    }while(0);  
} 

/**
 * [FilterIllgelChar description]
 * @param str [description]
 */
void FilterIllgelChar(char * str)
{
    int len = sizeof(str);

    for(int i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            str[i] = '\0';
        }
    }
}

/**
 * [RecvData description]
 * @param fd     [description]
 * @param events [description]
 * @param arg    [description]
 */
void RecvData(int fd, int events, void *arg)  
{  
    struct myevent_s *ev = (struct myevent_s*)arg;  
    // receive data  
    int len = recv(fd, ev->buff, sizeof(ev->buff)-1, 0);    
    EventDel(g_epollFd, ev);

    if(len > 0)
    {
        ev->len = len;
        ev->buff[len] = '\0';

        //FilterIllgelChar(ev->buff);
        LOG_INFO("%s:%d(%s), C[%d]:%s",__FILE__, __LINE__, __FUNCTION__, fd, ev->buff);

        int err = ProcessData(ev->buff);
        if(err)
        {
            /*code*/
        }
        // change to send event
        EventSet(ev, fd, SendData, ev);
        EventAdd(g_epollFd, EPOLLOUT|EPOLLET, ev);
    }  
    else if(len == 0)
    {
        close(ev->fd);
        LOG_INFO("%s:%d(%s),fd=%d closed gracefully.",__FILE__, __LINE__, __FUNCTION__, fd);
    }  
    else
    {
        close(ev->fd);

        LOG_WARN("%s:%d(%s), recv fd=%d, error=%d:%s",__FILE__, __LINE__, __FUNCTION__, 
            fd, errno, strerror(errno));
    }
}
 
/**
 * [SendData description]
 * @param fd     [description]
 * @param events [description]
 * @param arg    [description]
 */
void SendData(int fd, int events, void *arg)  
{  
    struct myevent_s *ev = (struct myevent_s*)arg;  
    int len;  
    // send data  
    len = send(fd, ev->buff, ev->len, 0);  
    ev->len = 0;  
    EventDel(g_epollFd, ev);  
    if(len > 0)
    {
        // change to receive event
        EventSet(ev, fd, RecvData, ev);
        EventAdd(g_epollFd, EPOLLIN|EPOLLET, ev);  
    }  
    else
    {  
        //有可能客户端没有来得及close就崩溃了
        close(ev->fd);

        LOG_WARN("%s:%d(%s), send fd=%d, error", __FILE__, __LINE__, __FUNCTION__, fd);
    }
}

/**
 * [GetBacklog description]
 * @return [description]
 */
int GetBacklog()
{
    char *ptr;
    int backlog = 5;
    if((ptr = getenv("LISTENQ")) != NULL)
    {
        backlog = atoi(ptr);
    }
    return backlog;
}

/**
 * [InitListenSocket description]
 * @param epollFd [description]
 * @param port    [description]
 */
void InitListenSocket(int epollFd, short port)  
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0); 
    int on = 1;

    //reuseaddr and no TIME_WAIT status
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    fcntl(listenFd, F_SETFL, O_NONBLOCK); // set non-blocking 
    LOG_INFO("%s:%d(%s), server listen fd:%d",  __FILE__, __LINE__, __FUNCTION__, listenFd);
    EventSet(&g_Events[MAX_EVENTS], listenFd, AcceptConn, &g_Events[MAX_EVENTS]);  
    // add listen socket
    EventAdd(epollFd, EPOLLIN|EPOLLET, &g_Events[MAX_EVENTS]);

    // bind & listen
    sockaddr_in sin;
    bzero(&sin, sizeof(sin));  
    sin.sin_family = AF_INET;  
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);  

    //for bindding error ,retry
    int times = 0;
    while(1){
        times++;
        if(0 == bind(listenFd, (const sockaddr*)&sin, sizeof(sin)))
        {  
            LOG_INFO("%s:%d(%s), server running:port:%d", __FILE__, __LINE__, __FUNCTION__, port);
            //cout << "backlog:" << GetBacklog()<< endl;
            listen(listenFd, GetBacklog());
            break;
        }
        else
        {
            LOG_ERROR("%s:%d(%s), bind error! retry %d times", __FILE__, __LINE__, __FUNCTION__,times);
            sleep(5);
        }
    } 
}


/**
 * [Exec description]
 */
bool Exec()
{
    short port = S_PORT; // default port  

    // create epoll  
    g_epollFd = epoll_create(MAX_EVENTS);
    if(g_epollFd <= 0) {
        LOG_ERROR("%s:%d(%s), create epoll failed.%d", __FILE__, __LINE__, __FUNCTION__, g_epollFd);
        return false;
    }

    // create & bind listen socket, and add to epoll, set non-blocking  
    InitListenSocket(g_epollFd, port);
    // event loop
    struct epoll_event events[MAX_EVENTS];

    int checkPos = 0;
    while(1){
        // a simple timeout check here, every time 100, better to use a mini-heap, and add timer event  
        long now = time(NULL);  
        for(int i = 0; i < 100; i++, checkPos++) // doesn't check listen fd  
        {
            if(checkPos == MAX_EVENTS) 
            {
                checkPos = 0; // recycle
            }
            if(g_Events[checkPos].status != 1) 
            {
                continue;
            }
            long duration = now - g_Events[checkPos].last_active;  
            if(duration >= TIME_OUT) // 60s timeout
            {
                close(g_Events[checkPos].fd);
                LOG_WARN("%s:%d(%s), fd: %d, timeout: %d--%d ", __FILE__, __LINE__, __FUNCTION__, 
                    g_Events[checkPos].fd, (int)g_Events[checkPos].last_active, (int)now);
                EventDel(g_epollFd, &g_Events[checkPos]);  
            }
        }

        // wait for events to happen
        // fds == 0 is timeout.
        int fds = epoll_wait(g_epollFd, events, MAX_EVENTS, 1000);  
        if(fds < 0){
            LOG_ERROR("%s:%d(%s), %s", __FILE__, __LINE__, __FUNCTION__, "epoll_wait error, exit");
            // free resource  
            close(g_epollFd);
            
            break; //continue
        }
        for(int i = 0; i < fds; i++){
            myevent_s *ev = (struct myevent_s*)events[i].data.ptr;
            if((events[i].events&EPOLLIN)&&(ev->events&EPOLLIN)) // read event  
            {
                ev->call_back(ev->fd, events[i].events, ev->arg);  
            }  
            if((events[i].events&EPOLLOUT)&&(ev->events&EPOLLOUT)) // write event  
            {  
                ev->call_back(ev->fd, events[i].events, ev->arg);  
            }  
        }
        
    }  
    
    return false;
}


