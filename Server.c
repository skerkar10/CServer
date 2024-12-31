#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Network includes
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>

//Multithreading
#include <pthread.h>

#define ERROR_COLOR "\x1b[31m"
#define RESET_COLOR "\x1b[0m"
#define BUFFER_SIZE 1024

void clearBuffers(char* a, char* b) {
  memset(a, 0, BUFFER_SIZE);
  memset(b, 0, BUFFER_SIZE);
}

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

  for (;;) {
    struct sockaddr Client = {0};
    socklen_t clientLen = sizeof(Client);

    int clientSfd = accept(socketfd, (struct sockaddr*)&Client, &clientLen);
    if (clientSfd < 0) {
      perror("Could not accept connection\n");
      continue;
    }

    printf("Connection successful on port# %d\n", atoi(argv[1]));

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    const char* initMessage = "Enter a message: ";
    const char* init = "Welcome to Sahil's server. Enter 'exit' to quit or 'close' to close server.\n";
    send(clientSfd, init, strlen(init), 0);

    for (;;) {
      clearBuffers(buffer, response);

      send(clientSfd, initMessage, strlen(initMessage), 0);
      ssize_t rec = recv(clientSfd, buffer, BUFFER_SIZE - 1, 0);
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
        close(socketfd);
        close(clientSfd);
        exit(0);
      }

      snprintf(response, BUFFER_SIZE, "Server received: %s\n", buffer);

      send(clientSfd, response, strlen(response), 0);
    }

    close(clientSfd);
  }

  close(socketfd);
  return 0;
}

