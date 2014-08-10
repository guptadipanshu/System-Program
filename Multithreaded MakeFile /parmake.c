/** @file parmake.c */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<string.h>
#include"queue.h"
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
 #include <semaphore.h>
/**
 * Entry point to parmake.
 */

typedef struct target_queue{
char *t_target;
char *t_dependency;
char *t_command;
int run;
} t_queue;

queue_t q,q2,q3,q4,q5,q6;
sem_t mutex;
int running;
pthread_mutex_t count_mutex;


void parsed_new_target(char *target){
  
    t_queue *item=malloc(sizeof(t_queue));
    // item->t_target=NULL;
    item->t_target=(char*)malloc(sizeof(char) * strlen(target)+1);
    strcpy(item->t_target,target);
    item->t_dependency=NULL;
    item->t_command=NULL;
    queue_enqueue(&q,item);    
    //item=queue_dequeue(&q);
   // printf("target= %s\n",(char*)item->t_target);
         
}
void parsed_new_dependency(char *target, char *dependency)
{
 	t_queue *item=malloc(sizeof(t_queue));
        item->t_target=(char*)malloc(sizeof(char) * strlen(target)+1);
        item->t_dependency=(char*)malloc(sizeof(char) * strlen(dependency)+1);
	  strcpy(item->t_target,target);
   	  strcpy(item->t_dependency,dependency);
          item->t_command=NULL;
          queue_enqueue(&q2,item);    
          //item=queue_dequeue(&q);
	//printf("returned target %s and dependency %s\n",item->t_target,item->t_dependency);
        
}

void parsed_new_command(char *target, char *command)
{   
	  t_queue *item=malloc(sizeof(t_queue));
	  item->t_target=(char*)malloc(sizeof(char) * strlen(target)+1);
	  item->t_command=(char*)malloc(sizeof(char) * strlen(command)+1);
	  strcpy(item->t_target,target);
   	  item->t_dependency=NULL;
          item->run=1;
          strcpy(item->t_command,command);
          queue_enqueue(&q3,item);    
          //printf("returned target %s and command %s\n",target,command);
}


void *thread_sort(void *ptr)
{        
	//int *result=malloc(sizeof(int));
          //  *result=1;
	 struct stat statbuf;
	int access_val;
	int i=0,j,k; int found=-1;
          
           t_queue* temp;
         if(queue_size(&q)>0)
           temp=queue_at(&q,0);
           char target3[4096];
           strcpy(target3,temp->t_target);
       pthread_mutex_lock(&count_mutex);   
    int value;
      while(queue_size(&q3)>0){
        pthread_mutex_unlock(&count_mutex);    
	// printf("we cannot enter");
     // sem_getvalue(&mutex, &value);
     // fprintf(stderr,"The value of the semaphore before the wait is %d\n", value);
      
      sem_wait(&mutex);
          
     
     //fprintf(stderr,"granted acces to CS %d\n",queue_size(&q3));
      found=0;
      int check=0;
      int find_dependency=1;
         if(queue_size(&q4)==1)
            {  find_dependency=1;
	       char *s=queue_remove_at(&q4,0);
                queue_enqueue(&q6,s);
		strcpy(target3,s);
                  //free(s);               
		//fprintf(stderr,"why");
            }
        if(queue_size(&q4)>1)
       {    
            char* s=queue_remove_at(&q4,0);
             strcpy(target3,s);
                  //free(s); 
             queue_enqueue(&q6,s);
            //printf("why");
            find_dependency=2;
       }
            
           if(queue_size(&q4)==0){
         for(i=0;i<queue_size(&q2);i++){
          // fprintf(stderr,"temp %s \n",target3);
	         t_queue* temp2=queue_at(&q2,i);
                    check=0; 
		 if(strcmp(target3,temp2->t_target)==0){//dependecy check
                       // printf("dependency is %s\n",temp2->t_dependency);
			access_val=access(temp2->t_dependency, F_OK);	
                           if(access_val!=0)
                             { strcpy(target3,temp2->t_dependency);i=-1;  check=1;}
                           if(access_val==0)
                              {
                                   //printf(" target is %s ",temp2->t_dependency);
                                    stat(temp2->t_dependency, &statbuf);
                                    char *str=ctime(&statbuf.st_mtime);
                                    
                                    //printf("Last file modification:   %s",str);
				    char * pch;
  				  //  printf ("Splitting string \"%s\" into tokens:\n",str);
                                    pch = strtok (str," ");
                                    for(k=0;k<4;k++)
 				    pch = strtok (NULL, " ");
  					
					  
					int a1=atoi(pch);
                                   
                                    for(k=0;k<queue_size(&q2);k++)
                                         {   
					     t_queue* temp3=queue_at(&q2,k);
		                               if(strcmp(temp2->t_dependency,temp3->t_target)==0){					     
					            access_val=access(temp3->t_dependency, F_OK);          
                                                    if(access_val==0)
                                                           {     //printf("\ndependency exist %s \n",temp3->t_dependency);
							         //printf(" target is %s ",temp3->t_dependency);
                                                                 stat(temp3->t_dependency, &statbuf);
                                                                 char *str2=ctime(&statbuf.st_mtime);
                                                                 char * pch2 = strtok (str2," "); 
                                                                      int d;
                                                                  for(d=0;d<4;d++)
 					                          pch2 = strtok (NULL, " ");  
                                                                 // printf ("%d\n",atoi(pch2)); 
                                                                  int a2=atoi(pch2); int h ;   char* remove_target=temp2->t_dependency;
                                                                  //fprintf(stderr," target to be removed %s %d %d",remove_target,a1,a2);
								   //printf("a1 is%d",a1); 
	
                                                                  if(a1>a2)
                                                                    {
                                                                           //fprintf(stderr,"we");   		
                      							    for(h=0;h<queue_size(&q2);h++)
                             						       {  t_queue* temp4=queue_at(&q2,h);
                                                                                    if(strcmp(remove_target,temp4->t_dependency)==0)
                                      						         {  temp4=queue_remove_at(&q2,h);
											    queue_enqueue(&q5,temp4);
											   h=-1;} 
                             					     	      }                          
                      							   for(h=0;h<queue_size(&q);h++)
                             							{  t_queue* temp4=queue_at(&q,h);
											if(strcmp(remove_target,temp4->t_target)==0)
                                    							 {  temp4=queue_remove_at(&q,h); 
											       queue_enqueue(&q5,temp4);	       							    
												h=-1;}
                           							} 
									for(h=0;h<queue_size(&q3);h++)
                             							{  t_queue* temp4=queue_at(&q3,h);
											if(strcmp(remove_target,temp4->t_target)==0)
                                    							 {  temp4=queue_remove_at(&q3,h); 
											    queue_enqueue(&q5,temp4);
				       							    h=-1;}
                           							}
                                                                             
                                                                    }
							   }                     

                                              }//strcmp ac "[Stage 2](#1): This should run as part of \"Stage 2\", alwayscess ==0
					 }
                                            
			       
                              }//acces val==0
                        }//strcmp for 1st for
	           }//end of 1st for
                 }
         //printf("temp outside check=0 %s\n",target3);
                
            if(check==0){
            //   fprintf(stderr,"completed %s\n",target3);
                int count;
		for(count=0;count<queue_size(&q3);count++){
                      t_queue* temp4=queue_at(&q3,count);
                         
                         //printf(" my temm %s at",temp4->t_command);
                      if((strcmp(temp4->t_target,target3)==0) && temp4->run==1){
                        found++;
                       //  fprintf(stderr,"command %s executed j value %d \n",temp4->t_command,count);
                              
			int ret= system(temp4->t_command);
                          temp4->run=2;
                          count=-1;
			 if(ret!=0)
                          {exit(1);}
                        // if(ret==0)
                          
                         }//strcmp  
                         
		 }// for loop
         pthread_mutex_lock(&count_mutex);
                          int flag=-1;
                          //printf("%d \n",queue_size(&q3)); 
                        if(strcmp(target3,"all")==0){ int start;
                                  for(start=0;start<queue_size(&q3);start++)
                                               {  
						 t_queue*ready_item=queue_at(&q3,start);
					          if(strcmp(ready_item->t_target,target3)==0)
                                                        flag=0;     
                                               }
                                   }
                         if(flag==-1){      
                          for(i=0;i<queue_size(&q2);i++)
                             {  t_queue* temp2=queue_at(&q2,i);
				if(strcmp(target3,temp2->t_dependency)==0)
                                     {  char* ready_target=temp2->t_target;
 					
				        temp2=queue_remove_at(&q2,i);  i=-1;
                                        // printf("ready targets %s\n", ready_target); 
                                            //free(temp2->t_dependency); 					
					    //free(temp2->t_target);
                                            //free(temp2);
                                              queue_enqueue(&q5,temp2);
                                          int start,present=0;
                                          
                                           for(start=0;start<queue_size(&q2);start++)
                                               {  t_queue*ready_item=queue_at(&q2,start);
					          if(strcmp(ready_item->t_target,ready_target)==0)
                                                      present=1;      
                                               }
                                            for(start=0;start<queue_size(&q2);start++)
                                               {  t_queue*ready_item=queue_at(&q2,start);
					          if(strcmp(ready_item->t_target,ready_target)==0)
                                                      present=1;      
                                               }
                                          if(present==0)
                                             {//printf("we insert in queue %s\n",ready_target);
                                               char* ready_target_insert=(char*)malloc(sizeof(char) * strlen(ready_target)+1);	
                                               strcpy(ready_target_insert,ready_target);				       
					       queue_enqueue(&q4,ready_target_insert);    
                                             }
					
				     }
                             } 
                            }                         
                         for(i=0;i<queue_size(&q);i++)
                             {  t_queue* temp2=queue_at(&q,i);
				        
					if(strcmp(target3,temp2->t_target)==0)
                                     {   
					temp2=queue_remove_at(&q,i); 
					queue_enqueue(&q5,temp2); i=-1;
					//free(temp2->t_target); 
					//free(temp2);
                                      }
                             }  

                    for(i=0;i<queue_size(&q3);i++)
                             {  t_queue* temp2=queue_at(&q3,i);
				        
					if(strcmp(target3,temp2->t_target)==0)
                                     {     
					temp2=queue_remove_at(&q3,i);
					//fprintf(stderr,"removing %s at i %d\n",temp2->t_command,i);
                                         queue_enqueue(&q5,temp2); 
					 i=-1;					
					
                                      }
                             }  
                             
                            
                        if(queue_size(&q)>0)                       
		         { temp=queue_at(&q,0);
                           strcpy(target3,temp->t_target);}
                   
       pthread_mutex_unlock(&count_mutex);
               }//if check==0
                      
                       
                    int f=queue_size(&q4);// printf("%d\n",f);
                  pthread_mutex_lock(&count_mutex);   
                 while(f!=0)
                    {  
		       sem_post(&mutex);
                       f--;
                    }
                 if(queue_size(&q3)==0)
                   {  sem_post(&mutex); }
                  pthread_mutex_unlock(&count_mutex);
  //                  sem_getvalue(&mutex, &value);
//  printf("The value of the semaphore after the wait is %d\n", value);   
             }//end of while loop 
       
return NULL;
}//end of tread function


int main(int argc, char **argv)
{     
	int index;
        int opt;
	int threads=1;
	int access_val;
	char *make_file_name=NULL;
	while ((opt = getopt(argc, argv, "f:j: ")) != -1) {
        switch (opt) {
        case 'f':
            make_file_name=optarg;
            break;
        case 'j':
            threads = atoi(optarg);
            break;
				   
       }
    }
int len=argc-optind;
char *targets[len];
int num_targets = 0; 
	 if(optind<argc){
                int j;
                    for(index=optind,j=0;index<argc;index++,j++){
                     	targets[j]=argv[index];
			num_targets++;
		     }
            }
	targets[num_targets] = NULL;

 if(make_file_name==NULL)
      {  make_file_name="./makefile";
         access_val=access(make_file_name, F_OK);
          if(access_val!=0){
              	make_file_name="./Makefile";
         	access_val=access(make_file_name, F_OK);
              }
           if(access_val!=0)
              return -1;
       }     
     queue_init(&q);
     queue_init(&q2);
     queue_init(&q3);
     queue_init(&q4);
     queue_init(&q5);
     queue_init(&q6);
    //printf("threads=%d makefile=%s access val=%d targets\n",threads,make_file_name,access_val);
    parser_parse_makefile(make_file_name,targets,parsed_new_target,parsed_new_dependency,parsed_new_command);
    pthread_t* t_threads=malloc(sizeof(pthread_t)*threads);
     int i,j,flag=0;     
    sem_init(&mutex,0,0);
    pthread_mutex_init(&count_mutex, NULL);
	
  
     int *ret;

     for(i=0;i<queue_size(&q);i++){
           t_queue* start_target=queue_at(&q,i);
                  flag=0;
                 for(j=0;j<queue_size(&q2);j++)
                       {
                            t_queue* start_target_q2=queue_at(&q2,j);
                                 if(strcmp(start_target->t_target,start_target_q2->t_target)==0)
                                      flag=1;
					
                       }
                 if(flag==0)
                 {       //fprintf(stderr," puswed %s",start_target->t_target);
			 char* ready_target_insert=(char*)malloc(sizeof(char) * strlen(start_target->t_target)+1);	
                         strcpy(ready_target_insert,start_target->t_target);				       
                         queue_enqueue(&q4,ready_target_insert);   
	           sem_post(&mutex);
                 }
          }
        if(queue_size(&q4)==0)
               sem_post(&mutex);
        for(i=0;i<threads;i++)
 	        {   
                      pthread_create(&t_threads[i],NULL,thread_sort,NULL);
                 }
        for(i=0;i<threads;i++){
            
               pthread_join(t_threads[i],(void**)&ret);
     
                  free(ret); 
               }
          free(t_threads);
         
        char *data;
    t_queue* temp5;
    
    while(queue_size(&q)!=0){
    temp5=queue_dequeue(&q);
    free(temp5->t_target);
    free(temp5);}
   while(queue_size(&q2)!=0){
    temp5=queue_dequeue(&q2);
    free(temp5->t_target);
    free(temp5->t_dependency);
    free(temp5);}
    while(queue_size(&q3)!=0){
    temp5=queue_dequeue(&q3); 
     free(temp5->t_target); 
    free(temp5->t_command); 
    free(temp5); } 
    while(queue_size(&q4)!=0){
  data=queue_dequeue(&q4); 
              free(data);}
   while(queue_size(&q5)!=0){
       temp5=queue_dequeue(&q5);
          if(temp5->t_target!=NULL) 
          free(temp5->t_target); 
          if(temp5->t_command!=NULL) 
          free(temp5->t_command); 
          if(temp5->t_dependency!=NULL) 
           free(temp5->t_dependency); 
    free(temp5);  
              }
      while(queue_size(&q6)!=0){
       data=queue_dequeue(&q6); 
              free(data);
                }
    pthread_mutex_destroy(&count_mutex);	    
    sem_destroy(&mutex); 
    return 0; 
}
