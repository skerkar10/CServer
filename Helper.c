#include "Helper.h"

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
      pthread_mutex_unlock(&mutex);
      serverRunning = 0;
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
    char* closingBuffer = "\nServer was closed! Goodbye.\n";
    send(currentClient->socket, closingBuffer, strlen(closingBuffer), 0);
  }

  close(currentClient->socket);
  free(currentClient);
  return NULL;
}
