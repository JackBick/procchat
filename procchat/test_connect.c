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
#include <wait.h>

int main(int argc, char** argv) {
    int pipe=open("gevent",O_WRONLY|O_NONBLOCK);
    
    short type = 0;
    char ident[256]="aaa";
    char domain[1790]="dom";
    char send_this[2048];
    memmove(send_this,&type, 2);
    memcpy(&send_this[2],&ident, 256);
    memcpy(&send_this[258],&domain, 1790);
    
    //2 bytes representing 1
    //256 ascii identifier
    //1790 char of domain

    write(pipe,send_this,2048);
    struct dirent * d_;
    DIR * s_ = opendir("./");

    while ((d_ = readdir(s_)) != NULL) {
        if (strcmp(d_->d_name, "dom")==0){
            printf("test_domain open\n"); 
            closedir(s_);           
            exit(0);
        }
    }
    printf("test_domain not there\n");
    exit(0);
}