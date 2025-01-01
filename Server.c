//Multithreading
#include <pthread.h>
#include <stdlib.h>

//Helper files
#include "Helper.h"

volatile int serverActive = 1;

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, ERROR_COLOR "Enter only the desired port number as a command line argument!\n");
    return 1;
  }

  struct sockaddr_in Server = {0};
  socklen_t serverLen = sizeof(Server);

  Server.sin_port = htons(atoi(argv[1]));
  Server.sin_addr.s_addr = INADDR_ANY;
  Server.sin_family = AF_INET;

  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
    perror("Failed to create socket\n");
    return 1;
  }

  int bindRes = bind(socketfd, (struct sockaddr*)&Server, serverLen);
  if (bindRes < 0) {
    perror("Failed to bind to socket\n");
    return 1;
  }

  const int ONE = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &ONE, sizeof(ONE));

  int lisRes = listen(socketfd, 10);
  if (lisRes < 0) {
    perror("Failed to listen on socket\n");
    return 1;
  }

  printf("Listening for connection on port# %d\n", atoi(argv[1]));

  while (serverActive) {
    struct sockaddr Client = {0};
    socklen_t clientLen = sizeof(Client);

    int clientSfd = accept(socketfd, (struct sockaddr*)&Client, &clientLen);
    if (clientSfd < 0) {
      perror("Could not accept connection\n");
      continue;
    }

    printf("Connection successful on port# %d\n", atoi(argv[1]));

    ClientADT* currentClient = malloc(sizeof(ClientADT));
    currentClient->clientObj = Client;
    currentClient->socket = clientSfd;

    handleClient((void*)currentClient);

    close(clientSfd);
  }

  close(socketfd);
  return 0;
}
