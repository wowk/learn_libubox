#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <libubox/uloop.h>


#define thread_safe_output(fmt, ...) do{\
    flockfile(stdout);\
    fprintf(stdout, fmt, ##__VA_ARGS__);\
    funlockfile(stdout);\
}while(0)


void child_process_exit_callback(struct uloop_process* c, int ret)
{
    printf("My Child exited with status code %d\n", ret);
    uloop_end();
}

int main(int argc, char* argv[])
{
    uloop_init();

    struct uloop_process process;

    int pid = fork();
    if(pid == 0){
        thread_safe_output("I am Child Process And I am sleeping\n");
        for(int i = 0 ; i < 8 ; i ++){
            thread_safe_output("\tsleep %d seconds\n", i + 1);
            sleep(1);
        }
        thread_safe_output("I am wakeup And exit with code %d\n", 321);
        exit(321);
    }else{
        thread_safe_output("I am Parent process And I am waiting for my Child\n");
        memset(&process, 0, sizeof(process));
        process.pid = pid;
        process.cb = child_process_exit_callback;
        uloop_process_add(&process);
    }

    uloop_run();
    uloop_done();

    return 0;
}
