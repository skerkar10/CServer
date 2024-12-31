#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERROR_COLOR "\x1b[31m"
#define RESET_COLOR "\x1b[0m"
#define BUFFER_SIZE 1024

void clearBuffers(char* a, char* b);

#endif
