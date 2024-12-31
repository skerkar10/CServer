#include "Helper.h"

void clearBuffers(char* a, char* b) {
  memset(a, 0, BUFFER_SIZE);
  memset(b, 0, BUFFER_SIZE);
}
