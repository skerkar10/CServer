#include "Helper.h"

extern volatile int serverActive;

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

  while (serverActive) {
    clearBuffers(buffer, response);

    send(currentClient->socket, initMessage, strlen(initMessage), 0);
    ssize_t rec = recv(currentClient->socket, buffer, BUFFER_SIZE - 1, 0);

    if (rec <= 0) {
      printf(ERROR_COLOR "Client disconnected!\n" RESET_COLOR);
      break;
    }

    buffer[strcspn(buffer, "\n")] = 0;

    if (strcmp(buffer, "exit") == 0) {
      printf(ERROR_COLOR "Client quit!\n" RESET_COLOR);
      break;
    }

    if (strcmp(buffer, "close") == 0) {
      printf(ERROR_COLOR "Closing Server. Goodbye!\n");
      serverActive = 0;
      break;
    }

    snprintf(response, BUFFER_SIZE, "Server received: %s\n", buffer);

    send(currentClient->socket, response, strlen(response), 0);
  }

  free(currentClient);
  return NULL;
}
