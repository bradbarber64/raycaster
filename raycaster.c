#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int readFile()
{
  char line[128];
  char fileName[20] = "test.txt";
  FILE *fh;
  
  fh = fopen(fileName, "r");

  if(!fh)
    {
      printf("\nCannot locate file");
    }

}

int main()
{
  return 0;
}
