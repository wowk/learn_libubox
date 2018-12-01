#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libubox/uloop.h>


void stdin_handler(struct uloop_fd* u, unsigned int events)
{
    size_t total;
    ssize_t len;
    char buf[128];

    total = 0;
    while(1){
        len = read(u->fd, buf, sizeof(buf)-1);
        if(len < 0){
            if(errno == EAGAIN)
                break;
            else if(errno == EINTR)
                continue;
            else{
                printf("failed to read from stdin: %s\n", strerror(errno));
                break;
            }
        }else if(len == 0){
            printf("Input End\n");
            uloop_end();
            return;
        }else{
            total += len;
            buf[len] = 0;
            printf("%s", buf);
        }
    }

    printf("\ntotal: %u bytes\n", total);
}

int main(int argc, char* argv[])
{
    uloop_init();

    struct uloop_fd fd;

    memset(&fd, 0, sizeof(fd));

    fd.fd = STDIN_FILENO;
    fd.cb = stdin_handler;
    uloop_fd_add(&fd, ULOOP_READ);

    uloop_run();
    uloop_done();

    return 0;
}
