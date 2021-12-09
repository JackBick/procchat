#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>

int main(int argc, char** argv) {
    struct dirent * dir_struct;
    DIR * dir_search = opendir("./");

    while ((dir_struct = readdir(dir_search)) != NULL) {
        if (strcmp(dir_struct->d_name,"gevent")==0){
            printf("gevent is open\n");
            int pipe=open("gevent",O_WRONLY|O_NONBLOCK);
            return 0;
        }
    }
    printf("gevent is not created\n");
    return 0;
}