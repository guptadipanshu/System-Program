/** @file log.c */
#include <stdlib.h>
#include <string.h>
#include "log.h"
/******************author@Dipanshu Gupta************/




void log_init(log_t *l) {
top=NULL;
l=NULL;
}

void log_destroy(log_t* l) {
l=top;
log_t *p=top;
//printf("\n destroy");
while(p!=NULL)
 { //printf("%s ",p->item1);
   log_t *nex =p->next;
   // if(p->item1)
  char *data=p->item1;
    free(data);
   
    free(p);    
    p=nex;      
 }
}

void log_push(log_t* l, const char *item) {
log_t *temp;
//printf("pushing %s ",item);
temp=(log_t*)malloc(sizeof(log_t));
temp->item1=(char*)item;
 if(top==NULL)
  {  //printf("top null");
    top=temp;
    top->next=NULL;
  }
else
  { //printf("top not null");
    temp->next=top;
    top=temp;
  }
l=top;
}
/*void display()
{
log_t *temp;
temp=top;
 while(temp!=NULL)
  {
    printf(" display %s  \n",temp->item1);
    temp=temp->next;
   }

}*/

/**
 * Preforms a newest-to-oldest search of log entries for an entry matching a
 * given prefix.
 *
 * This search starts with the most recent entry in the log and
 * compares each entry to determine if the query is a prefix of the log entry.
 * Upon reaching a match, a pointer to that element is returned.  If no match
 * is found, a NULL pointer is returned.
 *
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param prefix
 *    The prefix to test each entry in the log for a match.
 *
 * @returns
 *    The newest entry in the log whose string matches the specified prefix.
 *    If no strings has the specified prefix, NULL is returned.
 */
char *log_search(log_t* l, const char *prefix) {
int i=0;
l=top;
log_t *temp=top;
char *data;
data=temp->item1;
int j=0,k=0;
//j=strlen(data);
//k=strlen(prefix);
int check=0;
while(temp!=NULL)
 { data=temp->item1;
   j=strlen(data);
   k=strlen(prefix);
    for(i=0;i<k;i++)
     {
        if(j==k || j>k)
          {
            if(prefix[i]==data[i])
            check++;
            else
             { check=0;    
               break;//break out of for loop
             }
          }
     }
  if(check==0)
    {   temp=temp->next;
    //printf("\ntraversing next element");  
    }
  else 
   {  /* printf("\n did not traverse");*/
    break; //break out of while loop  
  } 
}//end of while loop
if(temp!=NULL)
  return temp->item1;
else
return NULL;
}
