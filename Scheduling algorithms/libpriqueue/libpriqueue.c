/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"




/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{   
   
   
   q->front=NULL;
   q->rear=NULL;
   q->pri=-1;
  // printf("data is %d\n",*((int*)comparer));
   //printf("intialised \n");
   q->comparer = comparer;
  //printf("%d",*(int*)q->comparer);
  
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{       
        
        int pos_stored=0;
	node_t* item =malloc(sizeof(node_t));
        node_t * temp=NULL;
        node_t* temp1=NULL;
        item->data=ptr;
        item->next=NULL;
	int a=-1;int c=0;
        int k;
        int ptr_value=*(int*)ptr;
        if(q->front !=NULL)
        {int front_value=*(int*)q->front->data;
        if(q->pri==-1)
      {	
	k= q->comparer(q->front->data,ptr);
        if(k!=0)
        q->pri=1; 
	//printf("k is %d front is %d ptr is %d \n",k,front_value,ptr_value);  
      }
        if(k<=0)
         {
         if(ptr_value<=front_value)
               { temp=q->front;
                 q->front=item;
                 q->front->next=temp;
                 pos_stored=0; 
                }  
              else
              {   temp1=q->front;
                  while(temp1!=NULL)
                  {
                        pos_stored++;
                        if(*(int*)temp1->data > ptr_value)
                          {     node_t *t =q->front;
                                int i;
                                pos_stored--;
				for(i=0;i<c-1;i++)
                                 t=t->next;
                                //printf("t is %d ",*(int*)t->data);
                                   node_t * newentry=item;
                                   newentry->next=t->next;
                                   t->next=newentry; 
                                  // printf("stored at position %d ",pos_stored);
                              a=0;
                              
                              break;                                 
                          }
                        c++;
                        temp1=temp1->next;
                  }
                  if(a==-1) 
		 { q->rear->next=item;
       		   q->rear=item;
                 }                  //printf("data is %d\n",*((int*)q->rear->data));
              }
        }
        


      if(k>0)
      {
         if(ptr_value>=front_value)
               { temp=q->front;
                 q->front=item;
                 q->front->next=temp;
                 pos_stored=0; 
                }


           else
              {   temp1=q->front;
                  while(temp1!=NULL)
                  {
                        pos_stored++;
                        if(*(int*)temp1->data < ptr_value)
                          {     node_t *t =q->front;
                                int i;
                                pos_stored--;
				for(i=0;i<c-1;i++)
                                 t=t->next;
                                //printf("t is %d ",*(int*)t->data);
                                   node_t * newentry=item;
                                   newentry->next=t->next;
                                   t->next=newentry; 
                                  // printf("stored at position %d ",pos_stored);
                              a=0;
                              
                              break;                                 
                          }
                        c++;
                        temp1=temp1->next;
                  }
                  if(a==-1) 
		 { q->rear->next=item;
       		   q->rear=item;
                 }                  //printf("data is %d\n",*((int*)q->rear->data));
              }

   
      }//end of k
 }//end of if
        if(q->front==NULL)
        {   pos_stored=0;
            q->front=item;
            q->rear=item;
          //printf("data is %d\n",*((int*)q->front->data));
        }
        
       
        
      
     
      /* temp1=q->front;
           while(temp1!=NULL)
            {   //printf("data is %p\n",temp1->data);
                printf("data is %d\n",*((int*)temp1->data));
                 temp1=temp1->next;
            }
           printf("\n");
     */
        //printf("stored at position %d\n",pos_stored);

        return pos_stored;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{      
        if(q->front==NULL)
	return NULL;
        else 
         return q->front->data;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{  
         if(q->front==NULL)
            return NULL;
      else{
         node_t * temp=NULL;
         node_t * temp2=NULL;
         temp2=q->front;
         temp=q->front->data;
         q->front=q->front->next;
         //printf("%p \n",temp2);
         
         return temp;  
      }
         

}


/**
  Returns the number of elements in the queue.
  
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{      node_t* temp1=NULL;
        int ct=0;
        temp1=q->front;
           while(temp1!=NULL)
            {   //printf("data is %p\n",temp1->data);
               // printf("data is %d\n",*((int*)temp1->data));
                     ct++;
                 temp1=temp1->next;
            }
        //printf("size is %d",ct);
	return ct;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
    node_t* temp=NULL;
    temp=q->front;
    //printf("destroy");
    while(temp!=NULL){
    node_t* nex =temp->next;
    //printf("destroy %d\n",*((int*)temp->data));
    free(temp);    
    temp=nex;         
    } 

}
