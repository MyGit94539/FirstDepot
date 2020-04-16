#include <stdio.h>
#include <stdlib.h>

#define arrSize 7

int arr[arrSize]={2,4,3,5,6,8,10};

int main(int argc, char *argv[])
{
	
	int i, j, k, t;

	for( i = 0; i < arrSize; i ++)
	{
		t= arr[i] * arr[i];
		arr[i] = t;
	}
	for( i = 0; i < arrSize-2; i ++)
	{
		for( j = i+1; j < arrSize -1; j ++)
		{
			t= arr[i] + arr[j];
			for( k = j+1; k < arrSize; k ++)
			{
				if (t == arr[k])
				{
					printf("%d %d %d\n", i, j, k);
						
				}
			}
		}
	}	

	return 0;
}

