#include <stdio.h>
#include <stdlib.h>

//swap bit 2 and bit 3
int main(int argc, char *argv[])
{
	unsigned int data = 0x0a;
	unsigned int bit1 = 0, bit2=0;

  bit1 = 1 << 3; 
  bit2 = 1 << 2;

	
//swap bit 2 and bit 3
	printf("data=  %x\n", data);
	
	if( ((data >> 2) & 0x01) != ((data >>3) & 0x01) )
			data = (data ^ bit1 ) ^ bit2;

	printf("data=  %x\n", data);

//set bit3
	
	printf("%x\n", data | bit1 );

//clear bit3
	data = 0x0f;
	if( (data >> 3 & 0x01 ) ==1)
	   printf("%x\n", data ^ bit1 );
}

