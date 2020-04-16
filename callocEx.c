#include <stdio.h> 
#include <stdlib.h> 
 
void main(void)
{
	int i, n;
	int *p;

	n = 5;
	p=(int *)calloc(n, sizeof(int));
	
	for(i =0; i < n; i++)
	{
		scanf("%d", &p[i]);
	}

	for(i =0; i < n; i++)
	{
		printf("%d \n", p[i]);
	}
	
	free(p);
}

 
