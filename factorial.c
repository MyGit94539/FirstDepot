#include <stdio.h> 
#include <stdlib.h> 
  

int main(int argc, char *argv[]) 
{ 
 
	int top;
	int factorial = 0;

	if( argc == 2 ) 
		top=atoi(argv[1]);

	factorial = top;
	while(--top)
	{
		factorial *=top;	
		printf("%d\n", factorial);	
	} 
	

    return 0; 
} 


