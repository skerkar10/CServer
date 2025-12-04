#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Multithreading
#include <pthread.h>

#include <netinet/in.h>
#include <sys/socket.h>

#define ERROR_COLOR "\x1b[31m"
#define RESET_COLOR "\x1b[0m"
#define GREEN_COLOR "\e[0;32m"
#define BUFFER_SIZE 1024

typedef struct {
  struct sockaddr clientData;
  int socket;
  int portNumber;
} ClientADT;

void clearBuffers(char* a, char* b);

void* handleClient(void* clientArgs);

#endif
