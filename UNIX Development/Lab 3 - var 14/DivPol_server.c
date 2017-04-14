/*
 * RPC-server 
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

//Get Real Power of polynom
int NormalizePower(Polynom pol)
{
	int result = 0;
	for (int i=pol.power;i>=0;i--)
	{
		if (pol.coefs[i] > 0.000001)
		{
			return i;
		}
	}
	return 0;
}

//polynoms division
void Divide(Polynom source, Polynom divider, Polynom* result, Polynom* remainder)
{
	remainder->power = source.power;
	for(int i=0;i<=remainder->power;i++)
		remainder->coefs[i] = source.coefs[i];
	
	result->power = remainder->power - divider.power;
	for (int i = 0; i <= result->power; i++)
	{
		double coeff = remainder->coefs[remainder->power - i] / divider.coefs[divider.power];
		result->coefs[result->power - i] = coeff;

		for (int j = 0; j <= divider.power; j++)
		{
			remainder->coefs[remainder->power - i - j] -= coeff * divider.coefs[divider.power - j];
		}
	}
	
	remainder->power = NormalizePower(*remainder);
	result->power = NormalizePower(*result);
}


//main on server
DivPolResponse* divpol_1_svc(DivPolRequest *request, struct svc_req *rqstp)
{
	static DivPolResponse  response;
	printf("Source = %s.\n",PolToString(request->source));
	printf("Divider = %s.\n",PolToString(request->divider));

	Divide(request->source,request->divider,&(response.result),&(response.remainder));
	
	printf("Result = %s.\n",PolToString(response.result));
	printf("Remainder = %s.\n",PolToString(response.remainder));
	
	return &response;
}
