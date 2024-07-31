#include <stdlib.h>

int main(int argc, char **argv)
{
  int c = 0;
  int nondeterm = 0, output = 0, i = 0;

  while (1)
  {
    if (c == 0)
    {
      nondeterm = atoi(argv[1]);
      c = 1;
    }
    else if (c == 1)
    {
      output = 0;
      c = 2;
    }
    else if (c == 2)
    {
      i = 0;
      c = 3;
    }
    else if (c == 3)
    {
      if (i < nondeterm)
      {
        c = 4;
      }
      else
      {
        c = 7;
      }
    }
    else if (c == 7)
    {
      return output;
    }
    else if (c == 4)
    {
      output += 2;
      c = 5;
    }
    else if (c == 5)
    {
      i++;
      c = 6;
    }
    else if (c == 6)
    {
      c = 3;
    }
  }

  return -1;
}