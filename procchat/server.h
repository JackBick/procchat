#ifndef SERVER_H
#define SERVER_H

void kill_function();

int child(char* domain, char* read_pipe,char* write_pipe, char* identifier);

#endif