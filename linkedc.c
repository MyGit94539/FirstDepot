#include <stdio.h>
#include <stdlib.h>

struct node {
	char c;
	struct node *next;
};

struct node *HEAD = NULL;
int nodeSize = sizeof(struct node);
int checkNode(char c)
{
	int nodeCnt = 0;
	struct node *p;

	p = HEAD;	

	while(p != NULL)
	{
		if( p->c == c)nodeCnt++;
		p=p->next;
	}
	return nodeCnt;	

} 
void removeNode(char c)
{
	struct node *p,*t,*oldt;
	p =  HEAD;	
	t = oldt = NULL;

	while(p != NULL)
	{
		if( p->c == c)
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
			{
				t->next = p->next;
			  p=t;
			}
		}
		oldt=t;
		t = p;
		p = p->next;
	}
}
void sortNode(void)
{
	char c;
	struct node *p,*t;

	p = t = HEAD;	

	while(p != NULL)
	{
		t=p->next;		
	
		while(t != NULL)
		{
			if( p->c > t->c)
			{
				c=p->c;
				p->c=t->c;
				t->c=c;
			}
			t=t->next;
		}
		p = p->next;
	}
}
void removeRepeatNode(void)
{

	struct node *p,*t,*tmp;

	p = HEAD;	

	while(p != NULL)
	{
			t=p->next;
			if( p->c == t->c )
			{
					p->next = t->next;
			}
  		p = p->next;
	}
}
void insertNode(char c)
{

	struct node *p,*t,*tmp;

	tmp = (struct node *)malloc(nodeSize);
	tmp->c=c;
	tmp->next = NULL;

	p=t=HEAD;

	while(p !=NULL)
	{
		if( tmp->c <= HEAD->c)
		{
			tmp->next = HEAD;
			HEAD = tmp;
			break;
		}

		if( p->next == NULL && (tmp->c >= p->c) )
		{
				p->next=tmp;
				break;
		}

		t=p->next;

		if( p->c <= tmp->c && tmp->c <=t->c )
		{
			p->next = tmp;
			tmp->next=t;
			break;
		}
	  p=p->next;
	}
}
void dispNode(char *title)
{
	struct node *p;
	printf("\n%s :",title);
	p = HEAD;	
	while(p !=NULL)
	{
		printf("%c ", p->c);
		p=p->next;
	}
	
}
void addNode(void)
{
	char c;
	struct node *p, *t;
	
	printf("Enter a char (q) to exit\n");
	while(1)
	{	
		c = getchar();  //scanf("%c", &c);
		
		if( c=='\n')continue;

		if( c =='q')break;

		t= (struct  node *)malloc(nodeSize);
		
		if(HEAD == NULL)
			HEAD=t;
		else
			p->next = t;

		p = t;
		p->c = c;
		p->next  = NULL;

	}
	
}
int main(int argc, char *argv[])
{
	char c='0';

	addNode();
	dispNode("addNode");
	
	
	c='b';
	removeNode(c);
	dispNode("removeNode");

  sortNode();
	dispNode("sortNode");
	c='d';
	insertNode(c);
	dispNode("insertNod");

 removeRepeatNode();
	dispNode("removeRepeatNode");

	return 0;
}
