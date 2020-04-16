#include <stdio.h>
#include <string.h>
void replaceStr(char *s1, char *s2)
{
	char str[]="this is test  ";
	int i,j,k,temp;
 
	int len1 = strlen(str);
	int len2 = strlen(s1);
	
	char c,tempStr[len2];

	if( len2 > len1)printf("no such string\n");
	
	for (i = 0; i <= (len1-len2); i++)
	{
		temp = i;
		for( j = i; j < (i+len2); j++)
		{
			tempStr[j-i] = str[j];
		}
		
	  if( strcmp(s1, tempStr) == 0)
		{
			for( k=0; k < len2; k++)
			{
			//	c= s2[k];
				str[temp+k] =  s2[k];
				printf("string found-- %c %c\n",str[temp+k], s2[k] );
			}
		}
	}
	printf("string found-- %s\n",str);

}
void findStr(char *s)
{
	char *str="this is test";
	int i;
	int j;
char c;

	int len1 = strlen(str);
	int len2 = strlen(s);
	
	char tempStr[len2];

	if( len2 > len1)printf("no such string\n");
	
	for (i = 0; i <= (len1-len2); i++)
	{
		for( j = i; j < (i+len2); j++)
		{
			//c = str[j];
			tempStr[j-i] = str[j];

			//printf("%c ", tempStr[j] );
		}

		printf("%s \n", tempStr );

		if( strcmp(s, tempStr) == 0)printf("string found-- %s\n",s);
	}
}

int main(int argc, char *agrv[])
{

	findStr("test");
	replaceStr("this", "tset");
	
	return 0;
}
