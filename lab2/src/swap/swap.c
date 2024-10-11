#include "swap.h"

void Swap(char *left, char *right)
{
	char temp = *left;
  	*left = *right;
  	*right = temp;
}

int main() 
{
  char a = 'a';
  char b = 'b';
  Swap(&a, &b);
  printf("%c %c\n", a, b);
  return 0;
}
