#include "Helper.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

extern volatile int serverRunning;
extern pthread_mutex_t mutex;

void clearBuffers(char* a, char* b) {
  memset(a, 0, BUFFER_SIZE);
  memset(b, 0, BUFFER_SIZE);
}

void* handleClient(void* clientArgs) {
  ClientADT* currentClient = (ClientADT*) clientArgs;

  char buffer[BUFFER_SIZE];
  char response[BUFFER_SIZE];

  const char* initMessage = "Enter a message: ";
  const char* init = "Welcome to Sahil's server. Enter 'exit' to quit or 'close' to close server.\n";
  send(currentClient->socket, init, strlen(init), 0);

  while (serverRunning) {
    clearBuffers(buffer, response);

    send(currentClient->socket, initMessage, strlen(initMessage), 0);
    ssize_t rec = recv(currentClient->socket, buffer, BUFFER_SIZE - 1, 0);

    if (rec <= 0) {
      pthread_mutex_lock(&mutex);
      printf(ERROR_COLOR "Client disconnected!\n" RESET_COLOR);
      pthread_mutex_unlock(&mutex);
      break;
    }

    buffer[strcspn(buffer, "\n")] = 0;

    if (strcmp(buffer, "exit") == 0) {
      pthread_mutex_lock(&mutex);
      printf(ERROR_COLOR "Client quit!\n" RESET_COLOR);
      pthread_mutex_unlock(&mutex);
      break;
    }

    if (strcmp(buffer, "close") == 0) {
      pthread_mutex_lock(&mutex);
      printf(ERROR_COLOR "Closing Server. Goodbye!\n");
      serverRunning = 0;
      pthread_mutex_unlock(&mutex);

      /*
      * This will fix the bug of the server not closing instantly after a
      * close message. This will work by creating a quick connection to the
      * server to unblock the accept
      */
      int tempSock = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in tempServ = {0};
      socklen_t tempServLen = sizeof(tempServ);
      tempServ.sin_family = AF_INET;
      tempServ.sin_port = htons(currentClient->portNumber);
      tempServ.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

      int response = connect(tempSock, (struct sockaddr*)&tempServ, tempServLen);
      if (response < 0) {
        printf(ERROR_COLOR "\nFailed to terminate server. Please kill server process manually!\n" RESET_COLOR);
      }

      close(tempSock);

      break;
    }

    snprintf(response, BUFFER_SIZE, "Server received: %s\n", buffer);

    send(currentClient->socket, response, strlen(response), 0);
  }

  /*
  * Let all users know that someone else closed the server
  * instead of just randomly kicking them off.
  */
  if (serverRunning == 0) {
    const char* closingBuffer = "\nServer was closed! Goodbye.\n";
    send(currentClient->socket, closingBuffer, strlen(closingBuffer), 0);
  }

  close(currentClient->socket);
  free(currentClient);
  return NULL;
}
