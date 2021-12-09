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

#define global_pipe "gevent"
#define TIMEOUT -1
#define TYP_BUF_LEN 2
#define IDENT_LEN 256
#define DOMAIN_LEN 1790
#define MSG_LEN 2048
#define SAY_LEN 1790

void kill_function(){
    //use to disconnect
    int pid = wait(NULL);
    kill(pid,SIGKILL);
}


int child(char* domain, char* read_pipe,char * write_pipe, char* identifier) {
    //this is a child client 
    struct pollfd read_client[1];
    read_client[0].fd = open(write_pipe,O_RDONLY);
    read_client[0].events = POLLIN;

    char type_buffer[TYP_BUF_LEN];
    char message[2046];

    while (1) {
        //poll checking for input to my pipe
        int ret = poll(read_client, 1, TIMEOUT);
        if (ret == -1) {
            perror("Poll has failed\n");
        }
        else if (!(read_client[0].revents & POLLIN)){
            continue;
        }
        else {
            read(read_client[0].fd, type_buffer, TYP_BUF_LEN);
            if (type_buffer[0]==7){
                //DISCONNECT command
                kill(getppid(),SIGUSR1);
                close(read_client[0].fd);
                unlink(read_pipe);    
                unlink(write_pipe);
                remove(write_pipe);            
                remove(read_pipe);
                exit(0);
            }
            else if(type_buffer[0]==5){
                //PING
                //int pid = fork();
                short type =6;
                char pong_message[2];
                memcpy(&pong_message,&type,2);
                //write(client_handler.fd,pong_message,2);
                //wait 15 seconds for input, if not given disconnect
                int pong_check = poll(read_client,1,15000);
                if (pong_check == -1) {
                    perror("pong poll has failed\n");
                }
                //disconnect

            }
            else if(type_buffer[0]==2||type_buffer[0]==1){
                short type = 0;
                //SAY
                if (type_buffer[0]==2){
                   type = 4;
                }
                //SAYCONT
                else if (type_buffer[0]==1){
                   type = 3;
                }
                //SAY/CONT command
                read(read_client[0].fd, message, 2046);
                //SAY
                if (type_buffer[0]==1){
                    message[2045] = '\0';
                }
                char send_this[MSG_LEN];

                //copy message to be sent
                memmove(send_this,&type,TYP_BUF_LEN);
                memcpy(&send_this[TYP_BUF_LEN],identifier,IDENT_LEN);
                //SAYCONT command
                if (type_buffer[0]==2){
                    memcpy(&send_this[258],message,1789);
                    memcpy(&send_this[2047],&message[2045],1);
                }
                //SAY
                else{
                    memcpy(&send_this[258],message,1790);
                }

                //setup directory search
                struct dirent * dir_struct;
                DIR * dir_search = opendir(domain);

                while ((dir_struct = readdir(dir_search)) != NULL) {
                    //tokenise 
                    char * child_id = strtok(dir_struct->d_name,"_");
                    char * child_mode = strtok(NULL,"_");

                    if (strcmp(identifier,child_id)!=0 && 
                        child_mode != NULL && 
                        strcmp(child_mode,"RD")==0) {
                        //create path for write pipe
                        char directory_path[IDENT_LEN];

                        strcpy(directory_path,domain);
                        strcat(directory_path,"/");
                        strcat(directory_path,dir_struct->d_name);
                        strcat(directory_path,"_RD");
                        //create pipe
                        struct pollfd send_to_client[1];
                        send_to_client[0].fd=open(directory_path,O_WRONLY);
                        send_to_client[0].events=POLLOUT;

                        while (1){
                            int p_check = poll(send_to_client,1,TIMEOUT);
                            if (p_check==-1){
                                perror("send to handler failed");
                            }
                            else if (!(send_to_client[0].revents & POLLOUT)){
                                continue;
                            }
                            else{
                                //write to pipe RECV
                                int w_check = write(send_to_client[0].fd,send_this,MSG_LEN);
                                if (w_check == -1){
                                    continue;
                                }
                                close(send_to_client[0].fd);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    //sig checks whether we need kill a child
    signal(SIGUSR1,kill_function);

    //setting up gevent
    struct pollfd global[1];
    mkfifo(global_pipe, 0666);
    global[0].fd = open(global_pipe, O_RDONLY);
    global[0].events = POLLIN;

    char type_buffer[TYP_BUF_LEN];
    char identifier[IDENT_LEN+1];
    char domain[DOMAIN_LEN+1];

    while (1) {
        //poll checks for if the pipe receives some input
        int ret = poll(global, 1, TIMEOUT);
        if (ret == -1) {
            perror("Poll has failed\n");
        }
        else if (!(global[0].revents & POLLIN)){
            continue;
        }
        else {
            //read our input type (should be connect)
            read(global[0].fd, type_buffer, TYP_BUF_LEN);
            int buf = (int)type_buffer[1];
            if (buf == 0) {
                //read rest of our message
                read(global[0].fd, identifier, IDENT_LEN);
                identifier[IDENT_LEN] = '\0';
                read(global[0].fd, domain, DOMAIN_LEN);
                domain[DOMAIN_LEN] = '\0';
                mkdir(domain, 0666);
                char read_pipe[MSG_LEN];
                char write_pipe[MSG_LEN];
                //create a string that will later be a pipe
                strcpy(read_pipe, domain);
                strcpy(write_pipe, domain);
                strcat(read_pipe, "/");
                strcat(write_pipe, "/");
                strcat(read_pipe, identifier);
                strcat(write_pipe, identifier);
                strcat(read_pipe, "_RD");
                strcat(write_pipe, "_WR");
                mkfifo(read_pipe, 0666);
                mkfifo(write_pipe, 0666);

                //forking allows me to create a multitude of process, and by continuing for the parent I don't duplicate and create pointless classes
                int pid = fork();
                if (pid != 0) {
                    continue;
                }
                else {
                    int retval = child(domain, read_pipe, write_pipe, identifier);
                    if (retval == -1) {
                        continue;
                    }
                }
            }
        }
    }
}