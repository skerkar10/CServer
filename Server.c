#include "Helper.h"

volatile int serverRunning = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void throw(char* errorInput) {
  fprintf(stderr, ERROR_COLOR "%s\n", errorInput);
  exit(1);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    throw("Enter only the desired port number as a command line argument!");
  }

  struct sockaddr_in Server = {0};
  socklen_t serverLen = sizeof(Server);

  Server.sin_port = htons(atoi(argv[1]));
  Server.sin_addr.s_addr = INADDR_ANY;
  Server.sin_family = AF_INET;

  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
    throw("Failed to create socket");
  }

  int bindRes = bind(socketfd, (struct sockaddr*)&Server, serverLen);
  if (bindRes < 0) {
    throw("Failed to bind to socket");
  }

  const int ONE = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &ONE, sizeof(ONE));

  int lisRes = listen(socketfd, 10);
  if (lisRes < 0) {
    throw("Failed to listen on socket");
  }

  printf("Listening for connection on port# %d\n", atoi(argv[1]));

  while (serverRunning) {
    struct sockaddr Client = {0};
    socklen_t clientLen = sizeof(Client);

    int clientSfd = accept(socketfd, (struct sockaddr*)&Client, &clientLen);
    if (clientSfd < 0 && serverRunning) {
      printf(ERROR_COLOR "Could not accept connection from client!\n" RESET_COLOR);
      continue;
    }

    pthread_mutex_lock(&mutex);
    printf(GREEN_COLOR "Connection successful on port# %d\n" RESET_COLOR, atoi(argv[1]));
    pthread_mutex_unlock(&mutex);

    ClientADT* currentClient = malloc(sizeof(ClientADT));
    currentClient->clientData = Client;
    currentClient->socket = clientSfd;
    currentClient->portNumber = atoi(argv[1]);

    pthread_t currentThread;

    pthread_create(&currentThread, NULL, handleClient, (void*)currentClient);

    pthread_detach(currentThread);
  }

  pthread_mutex_destroy(&mutex);
  close(socketfd);
  return 0;
}

