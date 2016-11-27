/*
 ============================================================================
 Name        : ctest.c
 Author      : Ted
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
//#include <pthread.h>
#include <math.h>
#if 1
void swap22(int *c, int *d)
{
  int tmp = *c;
  *c =  *d;
  *d = tmp;
}
int swap2(void)// by reference
{
	   int a = 1;
	   int b = 2;
	   printf("before swap a = %d\n", a);
	   printf("before swap b = %d\n", b);
	   swap22(&a, &b);
	   printf("after swap a = %d\n", a);
	   printf("after swap b = %d\n", b);
}
void swap11(int c, int d)
{
  int tmp = c;
  c =  d;
  d = tmp;
}
int swap1(void) // by value
{
    int a = 1;
    int b = 2;
    printf("before swap a = %d\n", a);
    printf("before swap b = %d\n", b);
    swap11(a, b);
    printf("after swap a = %d\n", a);
    printf("after swap b = %d\n", b);
}
void find_min_max(void)
{
	int array[]={1,3,5,4,2,7,9,4};
	int i;
	int min,max;

	int arraysize=sizeof(array)/sizeof(array[0]);

	min=max=array[0];
	for (i = 0; i < arraysize; i++)
	{
		if( min > array[i]) min= array[i];
		if( max < array[i]) max=array[i];
	}
	printf(" min = %d, max=%d\n", min, max);

	printf("Array elements =%d \n", sizeof(array)/sizeof(array[0]));

}
int void_ptr(void)
{
	char  c='1';
	int   d=100;
	float f=25.05;

	void *ptr;

	ptr = &c;

	//int *pd;

	//pd=(int *)ptr;

	unsigned char *pc;
	pc=(unsigned char*)ptr;
	printf("c %x\n", *pc);


	return 0;
}
struct foo {
  int width;
  int height;
};

void swaps1(struct foo input) {
    int tmp = input.width;
    input.width = input.height;
    input.height = tmp;
}

int swaps(void)
{
    struct foo first;
    first.width = 1;
    first.height = 2;
    printf("before swap first.width = %d\n", first.width);
    printf("before swap first.height = %d\n", first.height);
    swaps1(first);
    printf("after swap first.width = %d\n", first.width);
    printf("after swap first.height = %d\n", first.height);
}
void swapsp1(struct foo *input) {
    int tmp = input->width;
    input->width = input->height;
    input->height = tmp;
}

int swapsp(void )
{
    struct foo first;
    first.width = 1;
    first.height = 2;
    printf("before swap first.width = %d\n", first.width);
    printf("before swap first.height = %d\n", first.height);
    swapsp1(&first);
    printf("after swap first.width = %d\n", first.width);
    printf("after swap first.height = %d\n", first.height);
}
void swap(int *nums)
{
    int tmp;

    tmp = nums[0];
    nums[0]= nums[1];
    nums[1]= tmp;
}

int swapa()
{
    int nums[3] = {1,2};
    printf("before swap nums[0] = %d\n", nums[0]);
    printf("before swap nums[1] = %d\n", nums[1]);
    swap(nums);
    printf("after swap nums[0] = %d\n", nums[0]);
    printf("after swap nums[1] = %d\n", nums[1]);
}
const volatile int *ptr00;
void ipplus(void)
{
	int *ip;
	int i=20;

	ip = &i;

	printf("%d \n",++*ip);

}

int test1(int i)
{
    printf("%d \n", i);
    return 0;
}
int test2(int i)
{
    printf("%d \n", i*10);
    return 0;
}
int func_pointer(void)
{
    int i;

    int (*fp[10])(int);  //declares the array

    fp[0] = test1;  //assings a function that implements that signature in the first position
    fp[1] = test2;  //assings a function that implements that signature in the first position

    for(i=0; i<2; i++)
        fp[i](i+10); //call the cuntion passing 10

    return 0;
}

#define SECOND_OF_YEAR (60 * 60 * 24 * 365)
#define MIN(A,B) ( A <= B ? A : B )
#define MAX(A,B) ( A >= B ? A : B )
void minTest(void)
{
	//unsigned long x= 123456789UL;
    //x= SECOND_OF_YEAR;
	//printf("%d \n", MIN(7,6));
	//printf("%d \n", MAX(7,6));
	int a=7;
	int *p=&a;
	printf("%d %d %d\n", ++*p, (*p)++,(*p)++ );

	printf("max %d \n", MAX((*p)++,7) );

}
void bubble_sort(void)
{

	int a[]={1,2,4,3,9,7,6,5,1};
	int num = sizeof(a) / sizeof (a[0]);
	int i, j, temp;

	for( i = 0; i < (num -1); i++)
	{
		for(j= i+1; j < num; j++)
		{
			if (a[i]<a[j])
			{
				temp=a[i];
				a[i]=a[j];
				a[j]=temp;
			}
		}
	}
	for( i=0; i < num; i++)
	printf(" %d ", a[i]);
	printf("\n");
}
#define BIT3 ( 0x01 << 3)
void set_clear_bit3(void)
{
	static int a = 0x00;
/*
	a= a | 0x08;
	printf("%x ", a);
	a= a & 0x0f7;
	printf("%x ", a);
*/
	a |= BIT3;
	printf("%x ", a);
	a &= ~BIT3;
	printf("%x ", a);
}
#define BIT0 (0x01)
void count_bit_one(void)
{
	unsigned int x=0x81818181;
	int i;
	int count=0;
	int max = sizeof(unsigned int) * 8;
	for ( i = 0; i < max; i++)
	{
		if(( x & BIT0 )==1)
		{
			count++;
		}
		x >>= BIT0;
	}
	printf("%d ", count);
}


void aplusb(void)
{
	int a = 5;
	int b = 5;

	printf("  %x ", a ^ b); //12 as well
}
int str_test()
{
	float a=3.143529;
	    printf("%2.1f\n", a);
	    return 0;
}
int ptr_test2()
{
    void *vp;
    char ch=74, *cp="JACK";
    int j=65;
    vp=&ch;
    printf("%c", *(char*)vp);
    vp=&j;
    printf("%c", *(int*)vp);
    vp=cp;
    printf("%s", (char*)vp+2);
    return 0;
}
void swapstr(char *s1, char *s2)
{
	char *t="12345678";
	//strcpy(t, s1);
	//strcpy(s1, s2);
	//strcpy(s2, t);

	printf("%s %s\n", s1, s2);
}

int str_swap()
{
    char *ss[2]={"Tedgao7", "jingliu"};

    swapstr(ss[0], ss[1]);

  //  strcpy(&ss[0], &ss[1]);

    printf("%s %s\n", ss[0], ss[1]);

    return 0;
}
#define BIT1 0x01
int printBit(void)
{
	unsigned int x = 0x0c0c;
	int i;
	int BitOneCount=0;
	int size=sizeof(x)* 8;

	for(i = size-1; i >=0 ; i--)
	{
		if( (x & BIT1)== 1 )
		{
			printf("%d",BIT1);
			BitOneCount++;
		}
		else
			printf("%d",0);

		 x >>=BIT1;
	}
	printf("\n%d\n",BitOneCount);

	return 0;
}
#endif

void struct_test(void)
{
	struct st {
		int a;
		char c;
	};
	struct st_bit{
		unsigned char a:2;
		unsigned char b:2;
		unsigned char c:2;
		unsigned char d:2;
	};
	union
	{
		unsigned char x;
		struct st_bit st1;
	} UN;

	struct {
		int a;
		char c;
	}MySt1;

	struct st MySt;
	MySt.a = 8, MySt.c = 'd';

	printf("%d %c\n",MySt.a, MySt.c );

	struct st_bit BIT;
	BIT.a=0; BIT.b=1;BIT.c=2;BIT.d=3;
	printf("%d %d %d %d\n",BIT.a,BIT.b,BIT.c,BIT.d );

	UN.st1.a=0;	UN.st1.b=1; 	UN.st1.c=2; UN.st1.d=3;
	printf("%d %d %d %d\n",UN.st1.a,UN.st1.b,UN.st1.c,UN.st1.d );

	UN.x = 0xff;
	printf("%d %d %d %d\n",UN.st1.a,UN.st1.b,UN.st1.c,UN.st1.d );

	struct st *ST1;
	ST1 = (struct st *)malloc( sizeof(struct st ) );

	ST1->a =20;
	ST1->c='G';
	printf("%d %c\n",ST1->a, ST1->c='G' );

	MySt1.a = 8, MySt1.c = 'd';
	printf("mySt1 %d %c\n",MySt1.a, MySt1.c );


}
struct ls0{
	int x;
	struct ls0 *next;
};
struct ls0 *talloc(void)
{
	return (struct ls0 *)malloc( sizeof(struct ls0));
}
void list_test(void)
{
	int d;
	struct ls0 *head, *p, *old;

	printf("\n enter a value:\n");

	head=talloc();

	old=head;
	scanf("%d", &d);
	old->x = d;

	while(scanf("%d", &d)!=EOF)
	{
		p = talloc();
		p->x = d;
		old->next=p;
		old=p;
	}
	old->next=NULL;


/*
	for(i = 0; i < 5; i++)
	{
		p=talloc();
		p->x= i;
		p->next = head;
		head=p;
	}
*/


	p = head;
	while( p !=NULL)
	{
		printf("%d ", p->x);
	    p=p->next;
	}
	free(head);
}
void str_ops(void)
{
		char s1[50] ="12 34 56 78";
		char s2[3]="56";
		int i, j, k;

		char s3[4]="56";

		printf("testing %s\n", s1);

		for( i = 0; i < strlen(s1)-strlen(s2); i++)
		{
			for(j=0; j< strlen(s2); j++)
			{
				s3[j]=s1[i+j];
			}
			if( strcmp(s2, s3) == 0) printf("match ! %s\n", s2);
		}
}
int main(void)

{
	str_ops();
	//list_test();
	// struct_test();
	//char *a[2]= {"testing1", "testing2" };
	//int b[2][2] = { {1,2}, {3,4} };

	//printf("%d %d %d %d  \n", b[0][0], b[0][1],b[1][0],b[1][1]);
	//printf("%s %s \n", a[0],a[1]);

	printBit();

   //sr_swap();  // no swap at all, passed
	//str_test();
	//aplusb();
	//count_bit_one();
	//set_clear_bit3();
	bubble_sort();
	//minTest();
	func_pointer();
	//int *ptr = malloc(sizeof(0));
	//printf("%x \n", ptr);
	//void_ptr();
	//find_min_max();
	//swap1();
	//swap2();
	//swaps();
	//swapa();
	//swapsp();
	//swapa();
	//ipplus();
	return EXIT_SUCCESS;
}

