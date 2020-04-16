#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define bufferSize 10
int h, t, cnt, max, full;
int buf[bufferSize];

void dispStatus(void)
{
	printf("Full: %d, empty: %d, size: %d\n", full,  (!full && ( h == t)), max );

}
void disp(void)
{
	for( int i = 0; i < bufferSize; i ++)
	printf(" %d ", buf[i]);
	printf("\n");
}
bool bufferEmpty(void)
{
	   return (!full && (h==t) );
}
void bufferInit(void)
{
	 int i;  
	 h = 0;
   t = 0;
	 cnt = 0;
   max = bufferSize;
	 full = false;
	
	for(i=0; i < bufferSize; i ++)
		buf[i]=0;	
}
void bufferReset(void)
{
   h = 0;
   t = 0;
	 cnt = 0;
	 full = false;
}
void retreatPointer(void)
{
	full = false;
	t = ( t + 1) % max;
}
void advancePointer(void)
{
	if(full)
 		t = (t+1) % max;

	h = (h + 1) % max;

	if( h == t) full = true;
}

int bufferWrite(int data)
{
    int r = -1;

    if(!full)
    {
        buf[h] = data;
        advancePointer();
        r = 0;
    }
    return r;
}
int bufferRead(int *data)
{
    int r = -1;
    if(!bufferEmpty())
    {
        *data = buf[ t ];
        retreatPointer();
        r = 0;
    }
    return r;
}
int main(int argc, char *argv[])
{
	int data;
	
	bufferInit();
  dispStatus(); 
	
  for( int i = 0; i < 10; i ++)
	{
    if( bufferWrite(i) == -1 )
		{
			printf("buffer is full now\n");
			break;
		}
	}
  disp();
		
	for( int i = 0; i < 12; i ++)
	{
		if( bufferRead(&data) == -1)
		{
			printf("buffer is empty now\n ");
			break;
		}
		printf("%d ", data);
	}
  for( int i = 0; i < 10; i ++)
	{
    if( bufferWrite(i+10) == -1 )
		{
			printf("buffer is full now\n");
			break;
		}
	}
  disp();
	
	return 0;
}
