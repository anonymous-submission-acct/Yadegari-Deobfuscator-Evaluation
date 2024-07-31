#include <stdlib.h>

int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);
  int output = 0;

  int *ptr = &nondeterm;

  for (int i = 0; i < nondeterm; i++)
  {
    if (!(i < *ptr))
    {
      output += 100;
    }
    else
    {
      output += 2;
    }
  }

  return output;
}