#include "revert_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RevertString(char *str)
{
	int len = strlen(str);
  char *reversed = malloc(len + 1);
  int j = 0;
  for (int i = len - 1; i >= 0; i--) {
    reversed[j++] = str[i];
  }
  reversed[j] = '\0';
  strcpy(str, reversed);
  free(reversed);
}
