#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node_tag
{
	int r_no;
	char name[50];
	char code[10];
	unsigned int mks;
	struct node_tag * next;
}node;

typedef enum status_code{FAILURE , SUCCESS, UPDATE} status;
typedef enum Boolean {TRUE,FALSE} bool;

node * MakeNode(int rno, char stu_name[], char sub_code[], unsigned int marks)
{
	node * nptr;
	nptr=(node *)malloc(sizeof(node));
	if(nptr!=NULL)
	{
		nptr->r_no=rno;
		strcpy(nptr->name,stu_name);
		strcpy(nptr->code,sub_code);
		nptr->mks=marks;
		nptr->next=NULL;	
	}
	return nptr;	
}

void printList(node * head)
{
	while(head!=NULL)
	{
		printf("Roll no: %d\nStudent name: %s\nSubject code: %s\nMarks: %d\n\n",head->r_no,head->name,head->code,head->mks);
		head=head->next;
	}
}

int CompareNode(node * nptr, node * lptr)
{
	int ret_val;
	if((nptr->r_no>lptr->r_no)||((nptr->r_no==lptr->r_no)&&strcmp(nptr->code,lptr->code)>0))
	{
		ret_val=1;
	}
	else if((nptr->r_no<lptr->r_no)||((nptr->r_no==lptr->r_no)&&strcmp(nptr->code,lptr->code)<0))
	{
		ret_val=-1;
	}
	else
	{
		ret_val=0;
	}
	return ret_val;
}

node * reverse(node * head)
{
	node * mid, * trail;
	mid=NULL;
	while(head!=NULL)
	{
		trail=mid;
		mid=head;
		head=head->next;
		mid->next=trail;
	}
	return mid;
}



status InsertNode(node **hptr, node * lptr)
{
	status sc= SUCCESS;
	node* newhead;
	node* head= *hptr;
	node * prev,* ptr;
	if(head==NULL)
	{
		newhead=lptr;
	}
	else
	{
		newhead=prev=ptr=head;
		while((ptr!=NULL)&&(CompareNode(ptr,lptr))==(-1))
		{
			prev=ptr;
			ptr=ptr->next;	
		}
		if(ptr==NULL)                        // if lptr is last node to be inserted                        
		{
			prev->next=lptr;
			lptr->next=NULL;
				
		}		
		else if(CompareNode(ptr,lptr)==0)   //condition of updation
		{
			strcpy(ptr->name,lptr->name);
			ptr->mks=lptr->mks;
			sc=UPDATE;
		}
		else 
		{
			if(prev!=ptr)
			{
				lptr->next=prev->next;          
				prev->next=lptr;
			}
			else
			{
				lptr->next=ptr;                                  //insertion at start
				newhead=lptr;
			}
		}
	}
	*hptr=newhead;
	return sc;
}

status Insertion(node** hpptr,int rno, char stu_name[], char sub_code[], unsigned int marks)         //head also has to be passed  ryt?
{
	status stat=SUCCESS;
	node* lptr;
	lptr= MakeNode(rno,stu_name,sub_code,marks);
	if(lptr==NULL)
	{
		stat=FAILURE;
	}
	else
	{
		stat=InsertNode(hpptr,lptr);
	}
	return stat;
}

status DeleteNode(node **hptr, node * lptr)
{
	status sc= SUCCESS;
	node* newhead, *ptr;
	node* head= *hptr;
	node * prev;                                                       //include condition of single node
	if(head==NULL)
	{
		sc=FAILURE;
	}
	else
	{
		newhead=prev=ptr=head;
		while((ptr!=NULL)&&(CompareNode(ptr,lptr))==(-1))
		{
			prev=ptr;
			ptr=ptr->next;	
		}
		if(ptr==NULL)                        // if lptr can't be found in the entire linked list.                       
		{
			sc=FAILURE;
		}		
		else if(CompareNode(ptr,lptr)==0)   //condition of deletion
		{
			if(ptr==head)
			{
				newhead=head->next;	
			}
			else
			{
				prev->next=ptr->next;
			}
			free(ptr);
		}
		else
		{
			sc=FAILURE;
		}
		*hptr=newhead;
	}

	return sc;
}


status Deletion(node** hpptr,int rno, char stu_name[], char sub_code[], unsigned int marks)         //head also has to be passed  ryt?
{
	status stat=SUCCESS;
	node* lptr;
	lptr= MakeNode(rno,stu_name,sub_code,marks);
	if(lptr==NULL)
	{
		stat=FAILURE;
	}
	else
	{
		stat=DeleteNode(hpptr,lptr);
	}
	return stat;
}


int GetNumRecords(node* lptr)
{
	int count=0;
	while(lptr!=NULL)
	{
		lptr=lptr->next;
		count++;
	}
	printf("\nThe number of active records is %d\n",count);
	return count;
}


bool isEmpty(node* head)
{
	bool ret_val;
	if(head==NULL)
	{
		printf("\nThe list is empty.\n");
		ret_val=TRUE;
	}
	else
	{
		printf("\nThe list is not empty.\n");
		ret_val=FALSE;
	}
	return ret_val;
}

node * CopyNode(node * lptr)
{
	node * nptr;
	nptr=MakeNode(lptr->r_no,lptr->name,lptr->code,lptr->mks);
	return nptr;
}

node * list_union(node * list1, node * list2)
{
	node * ptr1=list1, * ptr2=list2, * list3=NULL, * nptr;
	int n;
	
	while(ptr1!=NULL && ptr2!=NULL)
	{
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==(-1))
		{
			nptr=CopyNode(ptr1);
			nptr->next=list3;
			list3=nptr;	
			ptr1=ptr1->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==0)
		{
			nptr=CopyNode(ptr1);
			nptr->next=list3;
			list3=nptr;	
			ptr1=ptr1->next;
			ptr2=ptr2->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==1)
		{
			nptr=CopyNode(ptr2);
			nptr->next=list3;
			list3=nptr;	
			ptr2=ptr2->next;	
		}
			
	}
	while(ptr1!=NULL)
	{
		nptr=CopyNode(ptr1);
		nptr->next=list3;
		list3=nptr;	
		ptr1=ptr1->next;
	}
	while(ptr2!=NULL)
	{
		nptr=CopyNode(ptr2);
		nptr->next=list3;
		list3=nptr;	
		ptr2=ptr2->next;	
	}
	list3=reverse(list3);
	return list3;
	
}

node * list_intersection(node * list1, node * list2)
{
	node * ptr1=list1, * ptr2=list2, * list3=NULL, * nptr;
	int n;
	
	while(ptr1!=NULL && ptr2!=NULL)
	{
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==(-1))
		{	
			ptr1=ptr1->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==0)
		{
			nptr=CopyNode(ptr1);
			nptr->next=list3;
			list3=nptr;	
			ptr1=ptr1->next;
			ptr2=ptr2->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==1)
		{	
			ptr2=ptr2->next;	
		}
			
	}
	
	list3=reverse(list3);
	return list3;	
}

node * list_difference(node * list1, node * list2)
{
	node * ptr1=list1, * ptr2=list2, * list3=NULL, * nptr;
	int n;
	
	while(ptr1!=NULL && ptr2!=NULL)
	{
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==(-1))
		{
			nptr=CopyNode(ptr1);
			nptr->next=list3;
			list3=nptr;	
			ptr1=ptr1->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==0)
		{	
			ptr1=ptr1->next;
			ptr2=ptr2->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==1)
		{
			ptr2=ptr2->next;	
		}
			
	}
	while(ptr1!=NULL)
	{
		nptr=CopyNode(ptr1);
		nptr->next=list3;
		list3=nptr;	
		ptr1=ptr1->next;
	}
	
	list3=reverse(list3);
	return list3;	
}

node * list_symmetric_difference(node * list1, node * list2)
{
	node * ptr1=list1, * ptr2=list2, * list3=NULL, * nptr;
	int n;
	
	while(ptr1!=NULL && ptr2!=NULL)
	{
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==(-1))
		{
			nptr=CopyNode(ptr1);
			nptr->next=list3;
			list3=nptr;	
			ptr1=ptr1->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==0)
		{	
			ptr1=ptr1->next;
			ptr2=ptr2->next;
		}
		while(ptr1!=NULL && ptr2!=NULL && CompareNode(ptr1,ptr2)==1)
		{
			nptr=CopyNode(ptr2);
			nptr->next=list3;
			list3=nptr;	
			ptr2=ptr2->next;	
		}
			
	}
	while(ptr1!=NULL)
	{
		nptr=CopyNode(ptr1);
		nptr->next=list3;
		list3=nptr;	
		ptr1=ptr1->next;
	}
	while(ptr2!=NULL)
	{
		nptr=CopyNode(ptr2);
		nptr->next=list3;
		list3=nptr;	
		ptr2=ptr2->next;	
	}
	list3=reverse(list3);
	return list3;	
}


node * CreateList()
{
	status sc;
	FILE * fp;
	int roll,n=0;
	char stu_name[50],sub_code[10];
	unsigned int marks;
	node * head=NULL, * nptr;
	fp=fopen("data.txt","r");
	
	while(n!=EOF)
	{
		n=fscanf(fp,"%d",&roll);
		if(n!=EOF)
		{
			fscanf(fp,"%s",stu_name);
			fscanf(fp,"%s",sub_code);
			fscanf(fp,"%d",&marks);
			
			//printf("%d\n%s\n%s\n%d\n",roll,stu_name,sub_code,marks);
			nptr=MakeNode(roll,stu_name,sub_code,marks);
			sc=InsertNode(&head,nptr);
					
		}		
	}
	
	fclose(fp);
	return head;				
}

node * CreateList2()
{
	status sc;
	FILE * fp;
	int roll,n=0;
	char stu_name[50],sub_code[10];
	unsigned int marks;
	node * head=NULL, * nptr;
	fp=fopen("data2.txt","r");
	
	while(n!=EOF)
	{
		n=fscanf(fp,"%d",&roll);
		if(n!=EOF)
		{
			fscanf(fp,"%s",stu_name);
			fscanf(fp,"%s",sub_code);
			fscanf(fp,"%d",&marks);
			
			//printf("%d\n%s\n%s\n%d\n",roll,stu_name,sub_code,marks);
			nptr=MakeNode(roll,stu_name,sub_code,marks);
			sc=InsertNode(&head,nptr);
					
		}		
	}
	
	fclose(fp);
	return head;				
}


int main()
{
	int number;
	bool boolean;
	status sc1,sc2;
	node * head, * list2, * list3;
	head=CreateList();
	sc1=Insertion(&head,16,"kalyani","dmgt",78);
	printList(head);
	number=GetNumRecords(head);
	boolean=isEmpty(head);
	sc2=Deletion(&head,17,"archu","web",56);
	printList(head);
	
	list2=CreateList2();
	printf("\n Second list is\n");
	printList(list2);
	list3=list_symmetric_difference(head,list2);
	printf("symm_diff is \n");
	printList(list3);
	getch();
	return 0;
}
