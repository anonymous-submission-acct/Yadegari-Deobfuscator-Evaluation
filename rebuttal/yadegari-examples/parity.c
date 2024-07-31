#include <stdlib.h>

int foobar(int in)
{
  return in * 2 + 3;
}

int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);

  if ((nondeterm << 2) > 10)
  {
    if (foobar(nondeterm) < 10)
    {
      return nondeterm % 2;
    }
    else
    {
      return nondeterm % 2;
    }
  }
  else
  {
    return nondeterm % 2;
  }
}
