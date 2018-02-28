// The database has been maintained using B+ trees//

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#define m 5             // m is the maximum number of links possible from a B Tree node
#define DATA_SIZE 3
#define MIN_DATASIZE 2
#define MAX 4
#define MIN 2

int Num_rec=0;          // global variable for number of records.

typedef struct info_tag{
	char student_name[100];
	unsigned int roll_number;
	char address[1000];
	unsigned int semester_number;
	char department_name[10];
}info;                             // This is the student record structure

typedef struct union_tag{
	union btree{
		struct key_nodes{
			unsigned int r_no[m+1];    // if a key has count number of keys then it has count+1 number of pointers ranging
			struct union_tag * ptr[m+1];  // from 0 to count. The 0 th position in key array is left black so as to attach 
			int count;                       // smallest branch at that index.
		}key;
		struct data_nodes{
			info arr[DATA_SIZE+1];
			struct union_tag * next;    // at any point, there are 'dataCount' number of records in data node ranging from
			struct union_tag * prev;    // indices 0 to dataCount-1
			int dataCount;
		}data;

	}u;
	char flag;                     // entire tree is maintained using single type of tree node.
}record;                           // union is used to access it as Btree node and data node accordingly


// ------------------------------------------- SEARCH --------------------------------------------------------

void printInfo(record * root, int i)              // prints a single record.
{
	printf("----------\n");
	printf("Roll Number : %d\n",root->u.data.arr[i].roll_number);
	printf("Student Name : %s\n",root->u.data.arr[i].student_name);
	printf("Department Name : %s\n",root->u.data.arr[i].department_name);
	printf("Semester Number : %d\n",root->u.data.arr[i].semester_number);
	printf("Address : %s\n",root->u.data.arr[i].address);
	printf("----------\n");	
}


record * searchNode(record *root, unsigned int no)  // searches recursively and stops when root is a pointer pointing
{                                                   // to desired data node.
	if(root->flag=='k')
	{
		int i=1;
		while(root->u.key.r_no[i]<=no && i<=root->u.key.count)
		{
			i++;
		}
		root=searchNode(root->u.key.ptr[i-1],no);

	}
	return root;
}

void searchValueInDataNode(record *dptr, unsigned int val)  // searches for a value in data node once the estimated data 
{                                                           // node is reached.
	int i;
	if(dptr==NULL)
	{
		printf("Error\n");
	}
	else if(dptr->flag=='d')
	{
		int found=0;
		for(i=0;i<DATA_SIZE && found==0;i++)
		{
			if(dptr->u.data.arr[i].roll_number==val)  // stops when keys match.
			{
				found=1;
			}
		}
		if(found==0)
		{
			printf("KEY NOT FOUND\n");
		}
		else
		{
			printf("Details of the record are: \n");
			printInfo(dptr,i-1);
		}
	}

}

void searchFunction(record *root,unsigned int val)  // wrapping up both functions
{
	record *result;
	result=searchNode(root,val);
	searchValueInDataNode(result,val);

}
// -----------------------------------------------------------------------------------------------------

// ---------------------------- INSERTION ---------------------------------------------------------------

record * insert(record *root, info details, unsigned int no)  // creates first node as data node and once it is filled, it 
{                                                       // is split into two thus forming a key node alongwith.
	unsigned int medno;
	record * right_subtree;
	record * newroot;
	if(root==NULL)                   // inserting the very first record and setting links for doubly linked list.
	{
		record *nptr;
		nptr=(record *)malloc(sizeof(record));
		nptr->flag='d';
		nptr->u.data.arr[0]=details;
		nptr->u.data.dataCount=1;
		nptr->u.data.next=NULL;
		nptr->u.data.prev=NULL;
		root=nptr;
		Num_rec++;
		//printf("\nINSERTION SUCCESSFUL\n");
	}
	else if(PushDown(no,root,&medno,&right_subtree,details))// PushDown returns 1 if height of tree increases due to insertion
	{
		newroot=(record *)malloc(sizeof(record));
		newroot->flag='k';
		newroot->u.key.count=1;
		newroot->u.key.r_no[1]=medno;         // the key to be added as root is caught in medno, its right branch in 
		newroot->u.key.ptr[0]=root;           // right_subtree and left one in root.
		newroot->u.key.ptr[1]=right_subtree;
		return newroot;

	}
	return root;

}


void insertInDataNode(info details,record *root)  // inserts a particular record in data node at correct position.
{
	int i=root->u.data.dataCount-1;
	if(root->flag=='d')
	{
		while(root->u.data.arr[i].roll_number>=details.roll_number && i>=0)
		{
			root->u.data.arr[i+1]=root->u.data.arr[i];
			i--;
		}
		root->u.data.arr[i+1]=details;
	}
	root->u.data.dataCount++;
	//printf("\nINSERTION SUCCESSFUL\n");
	Num_rec++;
		
}

void insertInKey(unsigned int no,record *root,record *right_subtree,int pos) // a key to be inserted brings a pointer 
{                                                                        // to right branch along with itself.
	int i;
	for(i=root->u.key.count;i>pos;i--)           // pos is the position as which a key and ptr is to be inserted.
	{
		root->u.key.r_no[i+1]=root->u.key.r_no[i];
		root->u.key.ptr[i+1]=root->u.key.ptr[i];
	}
	root->u.key.r_no[pos+1]=no;
	root->u.key.ptr[pos+1]=right_subtree;
	root->u.key.count++;
}

void searchPosition(unsigned no,record *root,int *pos)  // searches position in a key node to find a way to reach to 
{                                                       // desired data node.
	if(root->flag=='k')
	{
		if(no<root->u.key.r_no[1])
		{
			*pos=0;                                     // pos saves the index of branch taken.
		}
		else
		{
			for(*pos=root->u.key.count;no<root->u.key.r_no[*pos];*pos=*pos-1);
		}

	}
}


// splits the key and saves the pointer to right node in medright and new element to be inserted at next level 
// as new median.

// medentry and medright are key and ptr pairs to be inserted at current level.
void splitKey(unsigned int medentry,record *medright,record *root,int pos, unsigned int *newmedian,record **newright)
{                                                    
	int i;
	int median;
	if(pos<=MIN)   // to ensure that the data node in which entry is going to be inserted has lesser number of records 
	{              // than the other one.
		median=MIN;
	}
	else
	{
		median=MIN+1;
	}
	*newright=(record *)malloc(sizeof(record));

	for(i=median+1;i<=MAX;i++)    // assigning values to newly created right node
	{
		(*newright)->u.key.r_no[i-median]=root->u.key.r_no[i];
		(*newright)->u.key.ptr[i-median]=root->u.key.ptr[i];
	}
	(*newright)->flag='k';
	(*newright)->u.key.count=MAX-median;
	
	root->u.key.count=median;
	if(pos<=MIN)
	{
		insertInKey(medentry,root,medright,pos);  // the entry at current level gets inserted accordingly.
	}
	else
	{
		insertInKey(medentry,*newright,medright,pos-median);
	}
	*newmedian=root->u.key.r_no[root->u.key.count];     // setting new median
	(*newright)->u.key.ptr[0]=root->u.key.ptr[root->u.key.count]; // setting 0th pointer for newright
	
	root->u.key.count--;

}

// splits leaf and sends the median to parent key node and also as first entry of newly formed right node

void splitLeaf(info details,record *root,record **right_subtree,unsigned int *no)
{
     info temp_arr[m+1];
     record *leaf_right;
     int i=0,j,k=0;
     while(details.roll_number>root->u.data.arr[i].roll_number && i<root->u.data.dataCount) // elements are copied to 
     {                                                     // temporary array and from there distributed between root and 
        temp_arr[i]=root->u.data.arr[i];                   // right subtree.
        i++;
     }
     temp_arr[i]=details;
     while(i<=root->u.data.dataCount)
     {
        temp_arr[i+1]=root->u.data.arr[i];
        i++;
     }
     
     *no=temp_arr[i/2].roll_number;            // setting the median to be sent to parent level.
     root->u.data.dataCount=(int)ceil((double)root->u.data.dataCount/2);
     
     leaf_right=(record *)malloc(sizeof(record));
     leaf_right->flag='d';
     leaf_right->u.data.dataCount=i-root->u.data.dataCount;
     
     for(j=0;j<i/2;j++)
     {
         root->u.data.arr[j]=temp_arr[j];

     }
     for(j=i/2;j<i;j++)
     {
        leaf_right->u.data.arr[k]=temp_arr[j];
        k++;
     }
     
     if(root->u.data.next!=NULL)
     {
     	(root->u.data.next)->u.data.prev=leaf_right;
	 }
     leaf_right->u.data.next=root->u.data.next;    // setting pointers of both nodes.
     root->u.data.next=leaf_right;
     leaf_right->u.data.prev=root;
     *right_subtree=leaf_right;
     
     Num_rec++;
     //printf("\nINSERTION SUCCESSFUL\n");

}

void update(record * root, info details, unsigned int i)     // update if found key is already present
{
	if(root->u.data.arr[i].roll_number==details.roll_number)
	{
		root->u.data.arr[i]=details;
		printf("\nUPDATION SUCCESSFUL\n");	
	}
	else
	{
		printf("Error in updation\n");
	}
}


int PushDown(unsigned int no, record *root, unsigned int *medno, record **right_subtree,info details)
{                               // recursively inserts key and ptr into key node after inserting data into datanode
	int pos,i=0;
	int found=0;
	if(root->flag=='d')
	{
		while(root->u.data.arr[i].roll_number<no && i<root->u.data.dataCount)
		{
			i++;
		}
		if(root->u.data.arr[i].roll_number==no)
		{
			update(root,details,i);
		}
		else
		{
			if(root->u.data.dataCount<DATA_SIZE)           // if dataCount is well below max then simply insert
			{
				insertInDataNode(details,root);
				return 0;
			}

			else
			{
				splitLeaf(details,root,right_subtree,medno); // else split and send key and ptr to prvious level.
				return 1;
			}
		}
	}
	else if(root->flag=='k')
	{
		searchPosition(no,root,&pos);
		if(PushDown(no,root->u.key.ptr[pos], medno,right_subtree,details))
		{
		    no=*medno;
			if(root->u.key.count<MAX)     // insert key acquired from next level to current level.
			{
				insertInKey(no,root,*right_subtree,pos);
				return 0;
			}
			else
			{
				splitKey(no,*right_subtree,root,pos,medno,right_subtree); // split and send key to previous level
				return 1;
			}
		}
		return 0;
	}
}
// -------------------------------------------------------------------------------------------------------

// --------------------------- DELETION -------------------------------------------------------------------


// pos is always the index of node on the right. Then it may be the donor or the receiver...
// root is always the pointer to parent. One of its children is deficient.

void MoveLeftKey(record *root,int pos)  // parent donates to right node and in turn receives from left node    
{
	int c;
	record *t;
	
	t=root->u.key.ptr[pos-1];
	t->u.key.count++;
	t->u.key.r_no[t->u.key.count]=root->u.key.r_no[pos];
	t->u.key.ptr[t->u.key.count]=(root->u.key.ptr[pos])->u.key.ptr[0];
	
	t=root->u.key.ptr[pos];
	root->u.key.r_no[pos]=t->u.key.r_no[1];
	t->u.key.ptr[0]=t->u.key.ptr[1];
	t->u.key.count--;
	
	for(c=1;c<=t->u.key.count;c++)
	{
		t->u.key.r_no[c]=t->u.key.r_no[c+1];
		t->u.key.ptr[c]=t->u.key.ptr[c+1];
	}
}


void MoveRightKey(record *root,int pos) // parent donates to left node and in turn receives from right node
{
	int c;
	record *t;
	
	t=root->u.key.ptr[pos];
	for(c=t->u.key.count;c>0;c--)
	{
		t->u.key.r_no[c+1]=t->u.key.r_no[c];
		t->u.key.ptr[c+1]=t->u.key.ptr[c];
	}
	
	t->u.key.ptr[1]=t->u.key.ptr[0];
	t->u.key.count++;
	t->u.key.r_no[1]=root->u.key.r_no[pos];
	
	t=root->u.key.ptr[pos-1];
	root->u.key.r_no[pos]=t->u.key.r_no[t->u.key.count];
	(root->u.key.ptr[pos])->u.key.ptr[0]=t->u.key.ptr[t->u.key.count];
	t->u.key.count--;
}


void CombineKey(record *root,int pos)    // nodes at pos and pos-1 are deleted and key at pos is removed from parent.
{
	int c;
	record * right, * left;              // key from parent is also taken into combined node
	right=root->u.key.ptr[pos];
	left=root->u.key.ptr[pos-1];             // all details are put from right to left without considering which one is
	left->u.key.count++;                     // deficient.
	
	left->u.key.r_no[left->u.key.count]=root->u.key.r_no[pos];
	left->u.key.ptr[left->u.key.count]=right->u.key.ptr[0];
	
	for(c=1;c<=right->u.key.count;c++)
	{
		left->u.key.count++;
		left->u.key.r_no[left->u.key.count]=right->u.key.r_no[c];
		left->u.key.ptr[left->u.key.count]=right->u.key.ptr[c];
	}
	
	for(c=pos;c<root->u.key.count;c++)
	{
		root->u.key.r_no[c]=root->u.key.r_no[c+1];
		root->u.key.ptr[c]=root->u.key.ptr[c+1];
	}
	
	root->u.key.count--;
	free(right);		
}



void MoveLeftLeaf(record *root,int pos)  // second entry from right node is given to parent and first one as last entry to
{                                        // left
	record *lhs;
	record *rhs;
	int c;
	
	lhs=root->u.key.ptr[pos-1];
	rhs=root->u.key.ptr[pos];
	
	lhs->u.data.arr[lhs->u.data.dataCount]=rhs->u.data.arr[0];
	lhs->u.data.dataCount++;
	root->u.key.r_no[pos]=rhs->u.data.arr[1].roll_number;
	for(c=1;c<rhs->u.data.dataCount;c++)
	{
		rhs->u.data.arr[c-1]=rhs->u.data.arr[c];
		
	}
	rhs->u.data.dataCount--;
}

void MoveRightLeaf(record *root,int pos)   // last entry from left is given as key as well as first entry to right.
{
	record *lhs;
	record *rhs;
	int c;
	lhs=root->u.key.ptr[pos-1];
	rhs=root->u.key.ptr[pos];
	
	for(c=rhs->u.data.dataCount-1;c>=0;c--)
	{
		rhs->u.data.arr[c+1]=rhs->u.data.arr[c];
	}
	
	rhs->u.data.dataCount++;
	rhs->u.data.arr[0]=lhs->u.data.arr[lhs->u.data.dataCount-1];
	root->u.key.r_no[pos]=rhs->u.data.arr[lhs->u.data.dataCount-1].roll_number;
	lhs->u.data.dataCount--;	
}

void CombineLeaf(record *root,int pos)  // all records are copied from right to left. key from parent is directly deleted.
{
	record *lhs,*rhs;
	int c;
	
	lhs=root->u.key.ptr[pos-1];
	rhs=root->u.key.ptr[pos];
	int elems=lhs->u.data.dataCount;

	for(c=0;c<rhs->u.data.dataCount;c++)
	{
	
		lhs->u.data.arr[elems]=rhs->u.data.arr[c];
		elems++;
	}
	for(c=pos+1;c<=root->u.key.count;c++)
	{
		root->u.key.r_no[c-1]=root->u.key.r_no[c];
		root->u.key.ptr[c-1]=root->u.key.ptr[c];
	}
	lhs->u.data.next=rhs->u.data.next;
	if(rhs->u.data.next!=NULL)
	{
		(rhs->u.data.next)->u.data.prev=lhs;
	}
	lhs->u.data.dataCount=elems;
	root->u.key.count--;
	free(rhs);
	
}

void DeleteFromLeafDirect(unsigned int target,record *root)  // the desired record is deleted from dataNode
{
	int pos,i=0;
	if(root->flag=='d')
	{
		while(root->u.data.arr[i].roll_number<target)
		{
			i++;
		}
		if(root->u.data.arr[i].roll_number>target)
		{
			printf("ENTRY TO BE DELETED IS ABSENT\n");
		}
		else if(root->u.data.arr[i].roll_number==target)
		{
			i++;
			for(i;i<root->u.data.dataCount;i++)
			{
				root->u.data.arr[i-1]=root->u.data.arr[i];
			}
			root->u.data.dataCount--;
			Num_rec--;
			printf("DELETION SUCCESSFUL");
		}
		
	}
}

//Pos is always the right node
int RestoreLeaf(unsigned int target,record *root,int pos) // restores min number of records in leaf in any leaf is deficient
{                                                         // root is the parent pointer.
	int k=0;
	int retval=0;
	if(pos==0)
	{
		if((root->u.key.ptr[1])->u.data.dataCount>MIN_DATASIZE) // move left if right sibling has > min records
		{
			MoveLeftLeaf(root,1);
		}
		else
		{
			CombineLeaf(root,1);
			retval=1;
		}
	}
	else if(pos==root->u.key.count)
	{
		if((root->u.key.ptr[pos-1])->u.data.dataCount>MIN_DATASIZE)
		{
			MoveRightLeaf(root,pos);      // move right if left sibling has > min records
		}
		else
		{
			CombineLeaf(root,pos);     // combine if no adjacent sibling has > min records and return 1 to continue restoring                   
			retval=1;
		}
	}
	else if((root->u.key.ptr[pos-1])->u.data.dataCount>MIN_DATASIZE)
	{
		MoveRightLeaf(root,pos);
	}
	else if((root->u.key.ptr[pos+1])->u.data.dataCount>MIN_DATASIZE)
	{
		MoveLeftLeaf(root,pos+1);
	}
	else
	{
		CombineLeaf(root,pos);
		retval=1;
	}
	return retval;
	
}

int RestoreKey(unsigned int target,record *root,int pos)
{
	int k=0;
	int retval=0;
	if(pos==0)
	{
		if((root->u.key.ptr[1])->u.key.count>MIN)
		{
			MoveLeftKey(root,1);  // move left if right sibling has > min records
		}
		else
		{
			CombineKey(root,1);
			retval=1;
		}
	}
	else if(pos==root->u.key.count)
	{
		if((root->u.key.ptr[pos-1])->u.key.count>MIN)
		{
			MoveRightKey(root,pos);     // move right if left sibling has > min records
		}
		else
		{
			CombineKey(root,pos);
			retval=1;
		}
	}
	else if((root->u.key.ptr[pos-1])->u.key.count>MIN)
	{
		MoveRightKey(root,pos);
	}
	else if((root->u.key.ptr[pos+1])->u.key.count>MIN)
	{
		MoveLeftKey(root,pos+1);
	}
	else
	{
		CombineKey(root,pos);      // combine if no adjacent sibling has > min records and return 1 to continue restoring 
		retval=1;
	}
	return retval;
}

int Delete_rec(unsigned int target,record *root)  // deletes record from leaf and recursively restores the tree.
{
	int pos,k,check_count;
	
	if((root->u.key.ptr[0])->flag=='d')
	{
		searchPosition(target,root,&pos);
		DeleteFromLeafDirect(target,root->u.key.ptr[pos]);
		check_count=(root->u.key.ptr[pos])->u.data.dataCount;
		
		if(check_count>=MIN_DATASIZE)   // if > min records present after deletion
		{
			return 0;
		}
		else
		{
			k=RestoreLeaf(target,root,pos);	 // returns the action indicated by restore function.
			return k;
			
		}
		
	}
	else
	{
		searchPosition(target,root,&pos);
		if(Delete_rec(target,root->u.key.ptr[pos]))
		{
			check_count=(root->u.key.ptr[pos])->u.key.count;
			if(check_count>=MIN)        // returns 0 if > min no of keys present after deletion
			{
				return 0;
			}
			else
			{
				k=RestoreKey(target,root,pos);
				return k;
			}
			
		}
		
		
	}
	return 0;
}

record * Delete(unsigned int target,record *root)
{
	record *oldroot;
	int k;
	if(root==NULL)
	{
		printf("Tree is empty\n");
	}
	else if(root->flag=='d')          // case when root comprises of single dataNode
	{
		DeleteFromLeafDirect(target,root);
		if(root->u.data.dataCount==0)
			root=NULL;
	}
	else if (root->flag=='k')
	{
		Delete_rec(target,root);     // case when height of tree reduces due to deletion
		if(root->u.key.count==0)
		{
			oldroot=root;
			root=root->u.key.ptr[0];
			free(oldroot);
		}
	}
	
	return root;
}

//------------------------------------------------------------------------------------------------------------

int GetNumRecords(record * root)  // global variable is returned
{
	printf("The number of records in the list is %d\n",Num_rec);
	return Num_rec;
}

int GetHeight(record * root)    // leaf nodes are taken into account and levels start from 0.
{
	if(root==NULL)
		return -1;
	else if(root->flag=='d')
		return 0;
	else if(root->flag=='k')
	{
		return 1+GetHeight(root->u.key.ptr[0]);
	}
}

void printTree(record *root)  // database is printed following the 'next' links of doubly linked list.
{
	int i;
	if(root==NULL)
	{
		printf("No records in the tree\n");
	}
	else if(root->flag=='d')
	{
		record *iptr;
		iptr=root;
		while(iptr!=NULL)
		{
			for(i=0;i<iptr->u.data.dataCount;i++)
				printInfo(iptr,i);
			printf("\n*****node ends*****\n");
			iptr=iptr->u.data.next;
		}
	}
	else
	{
		printTree(root->u.key.ptr[0]);  // function is called recursively till data node is reached.
	}
	
}


void Range_Search(record * root, unsigned int num1, unsigned int num2)  // 
{
	int i=0;
	record * data_ptr;
	if(root==NULL)
	{
		printf("Tree is Empty\n");	
	}
	else 
	{
		data_ptr=searchNode(root,num1); // pointer to dataNode containing num1 is returned.
		
		if(data_ptr->flag=='d')
		{
			while(data_ptr->u.data.arr[i].roll_number < num1)
			{
				i++;	
			}
			
			while(data_ptr!=NULL)    // all intermediate records are printed out using the doubly linked list.
			{
				while(i<data_ptr->u.data.dataCount && data_ptr->u.data.arr[i].roll_number<=num2)
				{
					printInfo(data_ptr,i);
					i++;	
				}
				data_ptr=data_ptr->u.data.next;
				i=0;	
			}	
		}
	}
}

// helper function to return a record of given details.
info MakeInfo(unsigned int roll, char stu_name[], char dept_name[], unsigned int sem_no, char addr[])
{
	info details;
	
	details.roll_number=roll;
	strcpy(details.student_name,stu_name);
	strcpy(details.department_name,dept_name);
	details.semester_number=sem_no;
	strcpy(details.address,addr);
	
	return details;
}

// function to read from file and create tree.
record * CreateTree()
{
	FILE * fp;
	unsigned int roll,n=0, sem_no;
	char stu_name[50],dept_name[10];
	char addr[50];
	record * root=NULL;
	info details;
	fp=fopen("tree_data.txt","r");
	
	while(n!=EOF)
	{
		n=fscanf(fp,"%d",&roll);
		if(n!=EOF)
		{
			fscanf(fp,"%s",stu_name);
			fscanf(fp,"%s",dept_name);
			fscanf(fp,"%d",&sem_no);
			fscanf(fp,"%s",addr);

			details=MakeInfo(roll,stu_name,dept_name,sem_no,addr);
			root=insert(root,details,details.roll_number);
					
		}		
	}
	
	fclose(fp);
	return root;				
}

// function to write back to the file.
void WriteBack(record * root)
{
	int i;
	FILE * fp;
	fp=fopen("tree_data.txt","w");
	while(root->flag!='d')
	{
		root=root->u.key.ptr[0];
	}
	while(root!=NULL)
	{
		i=0;
		while(i<root->u.data.dataCount)
		{
			fprintf(fp,"%d\n",root->u.data.arr[i].roll_number);
			fprintf(fp,"%s\n",root->u.data.arr[i].student_name);
			fprintf(fp,"%s\n",root->u.data.arr[i].department_name);
			fprintf(fp,"%d\n",root->u.data.arr[i].semester_number);
			fprintf(fp,"%s\n",root->u.data.arr[i].address);
			i++;		
		}		
		root=root->u.data.next;
	}
	
	fclose(fp);		
}

int main()
{
	record * root=NULL;
	info details;
	unsigned int r_no,sem;
	char stu_name[50],dept[10],addr[10];
	int lower_bound, upper_bound, k;
	int stop=0,choice;
	
	root=CreateTree();
	
	
	printf("**************** Welcome User! *******************\n");
	while(stop==0)
	{
		printf("------------------------------------------------------------------------------\n");
		printf("Choose your operation:\n");	
		printf("1. Insert into the tree\n");
		printf("2. Delete from the tree\n");
		printf("3. Search in the tree\n");
		printf("4. Get Number of Records\n");
		printf("5. Get height of tree\n");
		printf("6. Make a Range Search\n");
		printf("7. Print Database\n");
		printf("8: Exit\n");
		printf("------------------------------------------------------------------------------\n");
		
		scanf("%d",&choice);
		
		switch(choice)
		{
			case 1:
				printf("\nEnter details to insert:");
				printf("\nRoll number: ");
				scanf("%d",&r_no);
				printf("Student Name: ");
				scanf("%s",&stu_name);
				printf("Department Name: ");
				scanf("%s",dept);
				printf("Semester Number: ");
				scanf("%d",&sem);
				printf("Address:");
				scanf("%s",addr);
				k=Num_rec;
				details=MakeInfo(r_no,stu_name,dept,sem,addr);
				root=insert(root,details,details.roll_number);
				if(Num_rec>k)
					printf("\nINSERTION SUCCESSFUL\n");
				break;
			
			case 2:
				printf("\nEnter Roll Number of record to be deleted: \n");
				scanf("%d",&r_no);
				root=Delete(r_no,root);
				break;
				
			case 3:
				printf("\nEnter roll number of record of which details are desired: ");
				scanf("%d",&r_no);
				searchFunction(root,r_no);
				break;
				
			case 4:
				k=GetNumRecords(root);	
				break;
				
			case 5:
				k=GetHeight(root);
				printf("\nHeight of tree is %d",k);
				break;
				
			case 6:
				printf("\nEnter Lower Bound of range ");
				scanf("%d",&lower_bound);
				printf("Enter Upper Bound of range ");
				scanf("%d",&upper_bound);
				printf("\nElements in given range are\n");
				Range_Search(root,lower_bound,upper_bound);
				break;
				
			case 7:
				printTree(root);
				break;
				
			case 8:
				printf("\nSaving the database...");
				WriteBack(root);
				printf("\nExited successfully.");
				
				stop=1;
				break;
				
			default:
				printf("\nInvalid choice of operation");
		}	
	}
	return 0;	

}
