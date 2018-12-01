#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libubox/uloop.h>


void timeout_callback(struct uloop_timeout* t)
{
    static int count = 0;

    if( count++ ==  8){
        uloop_timeout_cancel(t);
        printf("timer canceled, end\n");
        uloop_end();
    }else{
        printf("timemout happend <%d>\n", count);
        /* we can also use this API to add a timer */
        uloop_timeout_set(t, 1000);
    }
}

int main(int argc, char* argv[])
{
    struct uloop_timeout timeout;

    uloop_init();
    
    memset(&timeout, 0 ,sizeof(timeout));
    timeout.time.tv_sec = 1;
    timeout.time.tv_usec = 1;
    timeout.cb = timeout_callback;

    uloop_timeout_add(&timeout);

    uloop_run();
    uloop_done();

    return 0;
}
