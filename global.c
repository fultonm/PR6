#include <stddef.h>
#include "global.h"

word_t get_mem_address(char *mem_string) {
  /* A little bit of edge case handling...
   * x3002 + strlen("x3002") - 4 = 3002
   * 3002 + strlen("3002") - 4 = 3002 */
  unsigned short address = strtol(mem_string + strlen(mem_string) - 4, NULL, 16);
  return translate_memory_address(address);
}

unsigned int get_mem_data(char *mem_string) {
  /* x3002 + strlen("x3002") - 4 = 3002
   * 3002 + strlen("3002") - 4 = 3002 */
  return strtol(mem_string + strlen(mem_string) - 4, NULL, 16);
}