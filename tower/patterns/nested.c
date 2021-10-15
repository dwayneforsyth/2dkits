#include <stdio.h>

int main(int argc,char ** argv)
{
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      printf("i*4+j = %d\n",i*4+j);
    }
  }
  return 0;
}
