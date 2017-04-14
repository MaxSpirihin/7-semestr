#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[])
{

	const char* prName;
	if (argc < 2)
	  prName = "proc.exe";
	else
	  prName = argv[1];

	pid_t p = fork();
	if(p ==-1)
	{
	    // Handle error
	    printf("Error on fork\n");
		return -1;
	}
	else if(p == 0)
	{
	    //we are the child - run the proccess
	    return execl(prName,prName,(char *)0);
	}
	else
	{
	    // We are the parent - wait and 
	    time_t start,end;
	    time (&start);

	    int status;
	    wait(&status); 

	    time (&end);

	      printf("Proccess is over.\n");
	      if (WIFSIGNALED(status) > 0)    
		printf("The program was stopped by signal. Code = %d.\n",status);
	      else 
		printf("Time of execution = %d seconds.\n",(int)(end -start));
	}

	return 0;
}
