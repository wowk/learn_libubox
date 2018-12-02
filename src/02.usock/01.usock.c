#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libubox/uloop.h>
#include <libubox/utils.h>
#include <libubox/list.h>
#include <libubox/usock.h>
#include <sys/socket.h>


/***************************************************
 * We use <usock> to create TCP/UDP/UNIX socket.
 * <usock_inet> is used to create TCP/UDP socket.
 * <usock> is equal to <usock_inet> except <usock>
 * support create UNIX socket.
 * ************************************************/
void server_read(struct uloop_fd* u, unsigned int event)
{
    ssize_t len;
    char buf[128];

    printf("[server] read data from socket\n");
    while(0 < (len = recv(u->fd, buf, sizeof(buf)-1, 0))){
        buf[len] = 0;
        printf("\trecved data: %s\n", buf);
    }
    if(len == 0){
        printf("\tremote client closed\n");
        printf("\tlocal server closed\n");
        uloop_fd_delete(u);
        uloop_end();
    }else if(len < 0){
        perror("\tserver_read");
    }
}

void server_accept(struct uloop_fd* u, unsigned int event)
{
    int connfd = accept(u->fd, NULL, NULL);
    struct uloop_fd* conn = (struct uloop_fd*)malloc(sizeof(struct uloop_fd));
    conn->cb = server_read;
    conn->fd = connfd;
    uloop_fd_add(conn, ULOOP_READ);
}

void server()
{
    uloop_init();

    int sock = usock(USOCK_SERVER|USOCK_IPV4ONLY|USOCK_TCP, "0.0.0.0", "12345");
    if(sock < 0){
        printf("server usock error\n");
        return;
    }

    struct uloop_fd fd;
    memset(&fd, 0, sizeof(fd));
    fd.fd = sock;
    fd.cb = server_accept;
    uloop_fd_add(&fd, ULOOP_READ);

    uloop_run();
    uloop_done();

    close(sock);
}

void client()
{
    sleep(1);

    int sock = usock(USOCK_IPV4ONLY|USOCK_TCP, "127.0.0.1", "12345");
    if(sock < 0){
        printf("client usock error\n");
        exit(0);
    }
    printf("[client] write data to socket\n");
    write(sock, "Hello World", 12);

    close(sock);
    exit(0);
}

int main(void)
{
    int pid = fork();
    if( pid < 0 ){
        perror("fork");
    }else if(pid == 0){
        client();
    }else{
        server();
    }
    return 0;
}
