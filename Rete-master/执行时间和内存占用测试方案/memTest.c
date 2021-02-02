#include <cstdio>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
 
int main(){
    pid_t executive = fork();
 
    if( executive < 0 ){
        printf("fork failed\n");
        return 0;
    }
    else if( executive == 0 ){
        //child process
        execl("./hello", "hello", NULL);
    }
    else{
        struct rusage rused;
        int status = 0;
        if( wait4( executive, &status, 0, &rused  ) < 0 ){
            printf("wait4 failed\n");
            return 0;
        }
 
        if( WIFEXITED(status) ){
             int time_usage = (rused.ru_utime.tv_sec * 1000 + rused.ru_utime.tv_usec / 1000);
             time_usage += ( rused.ru_stime.tv_sec * 1000 + rused.ru_stime.tv_usec / 1000 );
            int memory_usage = rused.ru_minflt * ( getpagesize() / 1024 );
            printf("Time used: %d MS Memory used: %d KB \n", time_usage, memory_usage);
 
        }
    }
    return 0;
}
