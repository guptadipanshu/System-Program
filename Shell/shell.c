/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"
#include <limits.h>
/******************author@Dipanshu Gupta************/
log_t Log;

/**
 * Starting point for shell.
 */
int main() {
int stop=1;
log_init(&Log);
pid_t pid =getpid();
char cwd[PATH_MAX];
size_t userlen = 0;
char *userinput=NULL;
char *result=NULL;

while(stop!=-1)
{
int check_free=0;
userinput=NULL;
fflush(stdout);
getcwd(cwd,PATH_MAX);
printf("(pid=%d)%s$ ", pid, cwd);//user path
getline(&userinput,&userlen,stdin);//read
char buffertext[PATH_MAX];
int k;
strcpy(buffertext,userinput);
k=strlen(buffertext);
char usertext[PATH_MAX];
int i;
for(i=0;i<k-1;i++)
{usertext[i]=buffertext[i];
}
usertext[i]='\0';
userinput[strlen(userinput)-1]='\0';
//log_push(&Log,usertext);

//log_push(&Log,"hell");
//end tester
            int found=0;
          if(usertext[0]=='!' && usertext[1]!='#')
    {   pid =getpid();                
        printf("Command executed by pid=%d\n", pid);
        int j,m;char searchquery[PATH_MAX];
         m=strlen(usertext);
        for(j=0;j<m-1;j++)
          searchquery[j]=usertext[j+1];
          searchquery[j]='\0';
           result=NULL;
           check_free=1;
          if(top!=NULL)
        result = log_search(&Log,searchquery);
	if (result == NULL) { printf("No Match\n"); 
                                   found=-1;       }
	else {
              printf("%s matches %s\n", searchquery, result);
            char *abc =malloc(sizeof(char)*(strlen(result)+1));
              strcpy(abc,result);
              log_push(&Log,abc);                
              if(strncmp(result,"cd ",3)==0)
                  found=2;
              else
                 found=3;  
             }

    }
     


 if((usertext[0]=='c' && usertext[1]=='d' && usertext[2]==' ') || found==2)
  {      
        int n,c;
        if(found==2)
        strcpy(usertext,result);
        n=strlen(usertext);
        char newdir[50];
        for(c=0;c<n-3;c++)
          newdir[c]=usertext[c+3];
          newdir[c]='\0';
             if(found!=2)
          log_push(&Log,userinput); 
          //printf("%s",newdir);
          int dir_result;
         // char *senddir=NULL;
         // strcpy(senddir,newdir);
          dir_result=chdir(newdir);
          pid =getpid();                
          printf("Command executed by pid=%d\n", pid);
          if(dir_result==0)
           {    
               //printf("%s \n",usertext);  
               //printf("succes changed directory");
           }
          else if(dir_result==-1)
            printf("%s: No such file or directory\n",newdir);
           
  }
  
 if(strcmp(usertext,"exit")==0)
  { pid =getpid();                
    printf("Command executed by pid=%d\n", pid); 
    log_push(&Log,userinput); 
 stop=-1; }

 //display oldest to latest for command !#
if(strcmp(usertext,"!#")==0) 
    { pid =getpid();                
      printf("Command executed by pid=%d\n", pid);
      log_t *temp;
      log_t *newtop;
      temp=top;
      newtop=NULL;
       // display();
        while(temp!=NULL)
           {  //printf("item is %s ",temp->item1);
             log_t *newstack;
             newstack=(log_t*)malloc(sizeof(log_t));
             newstack->item1=temp->item1;
             if(newtop==NULL)
               {
                 newtop=newstack;
                 newtop->next=NULL;
               }
              else
                {
                  newstack->next=newtop;
                  newtop=newstack;
                }
             temp=temp->next;
          }

         temp=newtop;
        while(temp!=NULL)
           {
             printf("%s\n",temp->item1);
             temp=temp->next;
           }
    //destroy the new stack
         temp=newtop;
        while(temp!=NULL)
      { //printf("%s ",p->item1);
          log_t *nex =temp->next;
          free(temp);    
          temp=nex;      
       }
    check_free=1;
      }//end of the if

  
  
   if(((userinput[0]!='!')&& (strncmp(userinput,"cd ",3)!=0) && (strcmp(userinput,"exit")!=0)) || (found==3))
     {         
             if(found==3)
             { strcpy(usertext,result);
               //printf("%saaa \n",usertext);
             } 
           else
             log_push(&Log,userinput);
          //printf("OTHER COMMAND\n");
       int len=strlen(usertext);
       char part1[PATH_MAX];
       char part2[PATH_MAX];
       part1[0]='\0';
       part2[0]='\0';
      int check_parse=0;
       for(i=0;i<len;i++)
           { if(usertext[i]==' ')
             check_parse=1;
            }
          if(check_parse==1)
        {   //printf("it is okay to parse");  
           for(i=0;usertext[i]!=' ';i++)
             part1[i]=usertext[i];
               part1[i]='\0';
        }
         int j=0;
         if(check_parse==1)
            {for(i=i+1;i<len;i++,j++)
             part2[j]=usertext[i];
            }
         part2[j]='\0';
            
            char *array[]={part1,part2,NULL};
            char *array2[]={usertext,NULL};
            //system(userinput);
          pid_t pID = fork();
            int exit_code;
          if (pID == 0)
          {  
             pid_t childpid=getpid();
             printf("Command executed by pid=%d\n", childpid);
             if(check_parse==1)
             execvp(part1,array);
             
             else
             execvp(usertext,array2);
              log_destroy(&Log);
             printf("%s: not found\n",usertext);
             exit(0);
          }
             else 
                   waitpid(pID,&exit_code,0);
            //printf("Finished with exitcode %d\n",exit_code);
          
     }
if(check_free==1)
 free(userinput);
   
}//end of main while 


log_destroy(&Log);
//free(userinput);
return 0;

}
