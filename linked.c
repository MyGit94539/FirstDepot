#include <stdio.h>
#include <stdlib.h>

struct node {
	int data;
	struct node *next;
};

struct node *HEAD = NULL;
int nodeSize = sizeof(struct node);
 
void removeNode(int d)
{
	struct node *p,*t,*oldt;
	p = HEAD;	
	t = oldt = NULL;

	while(p != NULL)
	{
		if( p->data == d)
		{
			if( p == HEAD)
			{
				HEAD = p->next;
			}	
			else if(p->next == NULL)
			{
				p=oldt;
				p->next = NULL;			
			}	
			else
				t->next = p->next;
		}
		oldt=t;
		t = p;
		p = p->next;
	}
}
void removeNode1(struct node *p, int d)
{
	struct node *t,*oldt;
//	p = HEAD;	
	t = oldt = NULL;

	while(p != NULL)
	{
		if( p->data == d)
		{
			if( p == HEAD)
			{
				HEAD = p->next;
			}	
			else if(p->next == NULL)
			{
				p=oldt;
				p->next = NULL;			
			}	
			else
				t->next = p->next;
		}
		oldt=t;
		t = p;
		p = p->next;
	}
}
void sortNode(void)
{
	int d;
	struct node *p,*t;
	p = t = HEAD;	

	while(p != NULL)
	{
		t=p->next;		
	
		while(t != NULL)
		{
			if( p->data > t->data)
			{
				d=p->data;
				p->data=t->data;
				t->data=d;
			}
			t=t->next;
		}
		p = p->next;
	}
}
void removeRepeatNode(void)
{

struct node *p,*t,*tmp;

	p = t = HEAD;	

	while(p != NULL)
	{
		t= p->next;		
		
		if(p->data == t->data)
		      p->next=t->next;

		p = p->next;
	}

}
void insertNode(int data)
{
	int d;
	struct node *p,*t,*tmp;

	tmp = (struct node *)malloc(nodeSize);
	tmp->data=data;
	tmp->next = NULL;

	p=t=HEAD;

	while(p !=NULL)
	{
		if( tmp->data <=HEAD->data)
		{
			tmp->next = HEAD;
			HEAD = tmp;
			break;
		}

		if( p->next == NULL && tmp->data >=p->data)
		{
				p->next=tmp;
				break;
		}

		t=p->next;

		if( p->data <= tmp->data && tmp->data <=t->data)
		{
			p->next = tmp;
			tmp->next=t;
			break;
		}
	  p=p->next;
	}
}
void dispNode(void)
{
	struct node *p;
	
	p = HEAD;	
	while(p !=NULL)
	{
		printf("%d\n", p->data);
		p=p->next;
	}
	printf("\n");
}
void addNode(void)
{
	int d = 0;
	struct node *p, *t;
	
	printf("Enter data (-1) to exit\n");
	while(1)
	{	
		scanf("%d", &d);
		if( d ==-1)break;

		t= (struct  node *)malloc(nodeSize);
		
		if(HEAD == NULL)
			HEAD=t;
		else
			p->next = t;

		p = t;
		p->data = d;
		p->next  = NULL;

	}
	
}
int main(int argc, char *argv[])
{
	int d=0;
	struct node *p;

	addNode();
	dispNode();
	
	d=33;
	removeNode(d);
	dispNode();

	sortNode();
	dispNode();

	d=25;
	insertNode(d);
	dispNode();

	//removeRepeatNode();
	//dispNode();
	

	return 0;
}
