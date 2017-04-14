/*
 * RPC-client 
 * Program do division of two polynoms
 */

#include "DivPol.h"

//turn Polynom into string
const char* PolToString(Polynom pol)
{
	
	char* result = (char *) malloc(pol.power*15);
	int i;
	for (i=0;i<=pol.power;i++)
	{
		int p = pol.power - i;
		double coef = pol.coefs[p];
		
		if (coef==0 && p!=0)
			continue;
		
		char* s = (char *) malloc(15);
		if (p>0)
			if (coef==1)
				sprintf(s,"x^%d + ",p);
			else
				sprintf(s,"%.2f*x^%d + ",coef,p);
		else
			sprintf(s,"%.2f",coef);
		
		if (i==0)
			strcpy(result,s);
		else
			strcat(result,s);
	}
	
	return result;
}

//enter polynom throw console
void PolFromConsole(Polynom* pol, const char* name)	
{
	printf("Enter the power of %s:\n",name);
	
	if (scanf("%d",&(pol->power)) != 1)
		{
			printf("Not a number. Program has stopped.\n");
			exit(1);
		}
	while (pol->power < 0 || pol->power >=POLYNOM_MAX_POWER)
	{
		printf("The power of polynom must be from 0 to %d. Enter again:\n",POLYNOM_MAX_POWER-1);
		if (scanf("%d",&(pol->power)) != 1)
		{
			printf("Not a number. Program has stopped.\n");
			exit(1);
		}
	}
	
	printf("Enter the coefs of %s starting with c0:\n",name);
	for (int i = 0;i<=pol->power;i++)
		if (scanf("%lf",&(pol->coefs[i])) != 1)
		{
			printf("Not a number. Program has stopped.\n");
			exit(1);
		}
}

//CheckIfPolynom Is incorrect (coef on max power cant be zero)
int PolIsIncorrect(Polynom* pol)
{
	if (pol->coefs[pol->power]==0)
		return 1;
	
	return 0;	
}


int main (int argc, char *argv[])
{
	// arguments check
	if (argc < 2) {
		printf ("usage: %s server_host [-l use literal]\n", argv[0]);
		exit (1);
	}
	
	// try handle to server
	char *host;
	host = argv[1];
	CLIENT *clnt = clnt_create (host, DIVPOL_PROG, DIVPOL_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}
	
	// data entrance
	Polynom source, divider;
	if (argc == 3 && strcmp(argv[2], "-l") == 0)
	{
		// Data by Literal
		source.power = 3;
		source.coefs[0]=2;
		source.coefs[1]=0;
		source.coefs[2]=7;
		source.coefs[3]=1;
		divider.power=1;
		divider.coefs[0]=-1;
		divider.coefs[1]=9;
	}
	else
	{
		// Menu for data entrance
		PolFromConsole(&(source),"source polynom");
		PolFromConsole(&(divider),"divider");
		if (PolIsIncorrect(&(source)) == 1 || PolIsIncorrect(&(divider)) == 1)
		{
			printf("Incorrect polynom\n");
			exit(1);
		}
	}
	printf("Source = %s.\n",PolToString(source));
	printf("Divider = %s.\n",PolToString(divider));
	
	// prepare data for server and do request
	DivPolResponse  *response;
	DivPolRequest  request;
	request.source = source;
	request.divider = divider;
	response = divpol_1(&request, clnt);
	if (response == (DivPolResponse *) NULL) {
		clnt_perror (clnt, "call failed");//request error
	} else {
		//request complete. Show result
		printf("Result = %s.\n",PolToString(response->result));
		printf("Remainder = %s.\n",PolToString(response->remainder));
		printf("%s = (%s)*(%s) + %s.\n",PolToString(source),PolToString(divider),
			   PolToString(response->result),PolToString(response->remainder));
	}
	
	//delete clnt and exit
	clnt_destroy (clnt);
	exit (0);
}
