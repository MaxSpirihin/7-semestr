#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

int main(void)
{
 
 printf("I am a test proccess. I have started.");
 
  time_t start,end;
  time (&start);
  //long unuseful calculations
  int i,j;
  for (i=0;i<20000;i++)
    for(j=0;j<40000;j++)
    {
      int a = i*i*i*i*i*j*j*j*j*j*j*j;
    }

  time (&end);
  printf("I am over. Time=%d\n",end-start);
 
}
