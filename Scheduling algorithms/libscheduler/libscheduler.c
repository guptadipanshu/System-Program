/** @file libscheduler.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
int compare1(const void * a, const void * b)
{
           //printf("called by a=%d b=%d \n",*(int*)a,*(int*)b);
	return ( *(int*)a - *(int*)b );
}
int compare3(const void * a, const void * b)
{                  int b1=*(int*)b;
                   int a1=*(int*)a ;
                        if (a1>b1)
           //printf("called by a=%d b=%d \n",*(int*)a,*(int*)b);
	return ( *(int*)b - *(int*)a );
          else 
            return ( *(int*)a - *(int*)b );    
    
                    
}
int vacant=-1;
int total_time=0;
int idle=0;
int scheme_used;
int run=-1;
int *tot_cores;
int no_cores;
typedef struct _job_t
{
int active;
int job_id;
int arrive_time;
int start_time;
int response_time;
int run_time;
int end_time;
int rem_time;
int pri;
int core_given;

struct _job_t *next;
} job_t;
job_t *front;
job_t *rear;
job_t *front1;
job_t *rear1;
priqueue_t q;

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{ 
        
        //printf("scheme is %d \n",scheme);
        front=NULL;
        rear=NULL;
	total_time=0;int i;
        no_cores=cores;
        tot_cores=malloc(sizeof(int)*cores);
        for(i=0;i<cores;i++)
        { tot_cores[i]=-1;
          //printf("coress=  %d ",tot_cores[i]);
         }
        scheme_used=scheme;
        if(scheme==0)
        priqueue_init(&q, compare1);
        else if(scheme==1 || scheme==2)
        priqueue_init(&q, compare3);
        else if(scheme==3 || scheme==4)
        priqueue_init(&q,compare3);
        else if(scheme_used==5)
        front1=NULL;
        rear1=NULL;        

}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.a
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */

  
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{       
	     if(idle!=0)
             idle=time-idle;
            
             total_time=total_time+running_time+idle;
             idle=0;
             job_t *item=malloc(sizeof(job_t));
             item->arrive_time=time;
             item->active=-1;
             item->run_time=running_time;
             item->pri=priority;
             item->job_id=job_number;
             item->core_given=-1;
             item->rem_time=running_time;
             item->next=NULL;
            if(front==NULL)
            {
            front=item;
            rear=item;
           // printf("once");

           }
           else
           { // printf("rear");
              rear->next=item;
              rear=item;
           }
           job_t *t=front;
        /*   while(t!=NULL)
            {
               printf("job id is %d run time %d \n",t->job_id,t->run_time);
               t=t->next;
            }
         */
        int *value=malloc(sizeof(int)); 
          
          if(scheme_used==0)
          *value=job_number;
          if(scheme_used==1)
          *value=running_time;
          if(scheme_used==3 || scheme_used==4)
          *value=priority;
          if(scheme_used==2)
          *value=item->rem_time;
            if(scheme_used!=5)
           priqueue_offer(&q,value);
             
         
          //printf("offer called\n");
           if(vacant==-1 && scheme_used!=2 && scheme_used!=4 && scheme_used!=5)
            { int i;
              int *val2= priqueue_poll(&q);
                //printf("poll called");
                //free(val2);
             item->active=1;
             item->start_time=time;
               for(i=0;i<no_cores;i++)
                   { if(tot_cores[i]==-1)
                          break;}
                 
                   tot_cores[i]=1;
                     int j,count=0;
                     for(j=0;j<no_cores;j++)
                        { if(tot_cores[j]==1)
                                 count++;
                         }
                     if(count==no_cores)
                         vacant=0;
                    
                //   printf("i value is %d",i);
                    if(i<no_cores)
                   return i;  
                    else 
                      return -1;  
            }
          if(scheme_used==2)
           {
              int val2;
         //    printf("value is %d\n",run);  
             if(vacant==-1)
               { //printf("vacan ");
                 val2=*((int *)priqueue_poll(&q));
                 item->active=1;
                 item->start_time=time;
                 run=item->job_id; int i;
                 item->response_time=time-item->arrive_time;
                 for(i=0;i<no_cores;i++)
                   { if(tot_cores[i]==-1)
                          break;}
                 
                   tot_cores[i]=1;
                     int j,count=0;
                     for(j=0;j<no_cores;j++)
                        { if(tot_cores[j]==1)
                                 count++;
                         }
                     
                     if(count==no_cores)
                         vacant=0;
			 if(i<no_cores)
                           {  item->core_given=i;
				return i;
                           }
                             
               }
              else
                {
                  
                job_t* temp2=front;
                  for(;temp2!=NULL;temp2=temp2->next)
                      {  int tr=-temp2->start_time+time;
                              tr=temp2->run_time-tr;
			 //printf(" job %d  arrival %d start %d rem %d \n",temp2->job_id,temp2->arrive_time,temp2->start_time,temp2->rem_time);
			if(temp2->core_given!=-1 && tr>item->rem_time)
                           break;}
                          if(temp2==NULL)
                             return -1;
                           if(temp2->start_time!=time) 
			{val2=time-temp2->start_time;
                        // printf("jobid %d time %d start_time%d \n",temp2->job_id,time,temp2->start_time);
                        val2=temp2->run_time-val2;}
                        else
                         val2=temp2->rem_time;
                    // printf("jobid %d time of val2 %d new item time %d coregiven %d \n",temp2->job_id,val2,running_time,temp2->core_given);
                      if(val2>running_time)
                       {    temp2->rem_time=val2;
			    val2=*((int *)priqueue_poll(&q));
                            *value=temp2->rem_time;
                             temp2->active=-1;
                             item->active=1;
                             item->start_time=time;
                             run=item->job_id;
                             item->response_time=time-item->arrive_time;
                             item->core_given=temp2->core_given;
                             temp2->core_given=-1;
			     priqueue_offer(&q,value);
                       //    printf("WE PREEMPT");
                            return item->core_given;
                       }
                 return -1;      
              }    
                      
           }//scheme 2
           


           if(scheme_used==4)
           {
              int val2;
         //    printf("value is %d\n",run);  
             if(vacant==-1)
               { val2=*((int *)priqueue_poll(&q));
                 item->active=1;
                 item->start_time=time;
                 run=item->job_id;
                  int i;
                 item->response_time=time-item->arrive_time;
                  for(i=0;i<no_cores;i++)
                   { if(tot_cores[i]==-1)
                          break;
                    }
                 
                   tot_cores[i]=1;
                     int j,count=0;
                     for(j=0;j<no_cores;j++)
                        { if(tot_cores[j]==1)
                                 count++;
                         }
                     
                     if(count==no_cores)
                         vacant=0;
			 if(i<no_cores)
                           {  item->core_given=i;
				return i;
                           }
                 
               }
              else
                {
                   
                job_t* temp2=front;
                  for(;temp2!=NULL;temp2=temp2->next)
                      {if(temp2->job_id==run )
                           break;}
                           if(temp2->start_time!=time) 
			{val2=time-temp2->start_time;
                        // printf("jobid %d time %d start_time%d \n",temp2->job_id,time,temp2->start_time);
                        val2=temp2->run_time-val2;}
                        else
                         val2=temp2->run_time;
                     //printf("time of val2 %d new item time %d \n",val2,running_time);
                      if(temp2->pri>item->pri)
                       {    temp2->rem_time=val2;
			    val2=*((int *)priqueue_poll(&q));
                            *value=temp2->pri;
                             temp2->active=-1;
                             item->active=1;
                             item->start_time=time;
                             run=item->job_id;
                             item->core_given=temp2->core_given;
                             temp2->core_given=-1;
                             item->response_time=time-item->arrive_time;
                             priqueue_offer(&q,value);
                       //    printf("WE PREEMPT");
                            return item->core_given;
                       }
                 return -1;      
              }    
                      
           }//scheme 4

          if(scheme_used==5)
            {
		  job_t *data=malloc(sizeof(job_t));
                  data->next=NULL;
                  data->job_id=job_number;
                  data->active=-1;	
		if(front1==NULL)
                  {front1=data;
                   rear1=data;
                  }
                 else
                   { rear1->next=data;
                     rear1=data;
                   }


                 job_t * temp2=front1;
                      while(temp2!=NULL)
                   {
                     // printf("job id %d\n",temp2->job_id);
                      temp2=temp2->next; 
                   }
                        temp2=front1;
                 if(temp2->job_id!=data->job_id)
                       return -1;
                    else
                       {
                       data->start_time=time;
                       data->active=1;
                       item->response_time=time-item->arrive_time;
                       return 0;
                       }
            }//scheme 5



               
          return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{           
        job_t * temp=front;
        int val2 =-1;
        int *val1;
        while(temp->job_id!=job_number)
          { 
             temp=temp->next;
          }
        if(temp->run_time+temp->start_time==time && total_time!=time && scheme_used!=2 && scheme_used!=4 && scheme_used!=5)
         { 
          temp->end_time=time;
            int size_queue=priqueue_size(&q);
              if(size_queue>0)
          // printf("poll called\n");
	  val2= *((int *)priqueue_poll(&q));
         
        //   free(val1);
           tot_cores[core_id]=-1;
            if(size_queue==0)
            { vacant=-1;
             }  
         if(scheme_used==0 && size_queue>0)
        {
           job_t* temp2=front;
           while(temp2->job_id!=val2)
              { 
               temp2=temp2->next;
              }
            
              temp2->start_time=time;
              tot_cores[core_id]=1;
         }

         if(scheme_used==1 && size_queue>0)
        {
           job_t* temp2=front;
            for(;temp2!=NULL;temp2=temp2->next)
              { 
                  if(temp2->run_time==val2 && temp2->active!=1)
                           break;
              }

	      temp2->start_time=time;
              temp2->active=1;
              val2=temp2->job_id;
              tot_cores[core_id]=1;
          }//scheme1
            if(scheme_used==3 && size_queue>0)
            {
               job_t* temp2=front;
               //printf(" val2 %d job_id %d ,",val2,temp2->job_id);
               for(;temp2!=NULL;temp2=temp2->next)
              { 
                  if(temp2->pri==val2 && temp2->active!=1)
                     break;
              }
              // printf("val2 is %d ",val2);
               
               tot_cores[core_id]=1;
               temp2->start_time=time;
               temp2->active=1;
               val2=temp2->job_id;
               
           }
           
        return val2;
      }
             if(scheme_used==2)
            {              temp->rem_time=0;
                            temp->core_given=-1;
                                temp->active=-1;
                               tot_cores[core_id]=-1;
                             int size_queue=priqueue_size(&q);
                              if(size_queue>0)
	                        { val2= *((int *)priqueue_poll(&q));
                                      job_t* temp2=front;
                                      for(;temp2!=NULL;temp2=temp2->next)
                                        { 
                                                //printf("val 2 %d job=%d time_rem=%d\n",val2,temp2->job_id,temp2->rem_time);
                                         if(temp2->rem_time==val2 && temp2->active!=1)
                                         break;
                                        }
                                         if(temp2!=NULL)
                                          {
                                         temp2->active=1;
	                                 temp2->core_given=core_id;
                                         tot_cores[core_id]=1;
					 temp2->start_time=time;
                                         temp->end_time=time;
              			         val2=temp2->job_id;
                                         run=temp2->job_id;
                                         if(temp2->run_time==temp2->rem_time)
                                         temp2->response_time=time-temp2->arrive_time;
                                          }
                                   return val2;
                                  }
				else
                                  {
                                   temp->end_time=time; 
                                   vacant=-1;
                                   return -1;
                                  }
                     
               }
        
                 if(scheme_used==4)
                      {
                               temp->core_given=-1;
                               tot_cores[core_id]=-1;
                               //temp->active=-1;
                                temp->end_time=time;
                             int size_queue=priqueue_size(&q);
                              if(size_queue>0)
	                        { val2= *((int *)priqueue_poll(&q));
                                      job_t* temp2=front;
                                      for(;temp2!=NULL;temp2=temp2->next)
                                       {// printf("val2 %d",val2);
                                         if(temp2->pri==val2 && temp2->active!=1)
                                         break;
                                        }
                                         temp2->core_given=core_id;
                                         tot_cores[core_id]=1;
                                         temp2->active=1;
	                                 temp2->start_time=time;
                                         temp->end_time=time;
                                         //printf("temp2->id =%d ",temp2->job_id);
              			         val2=temp2->job_id;
                                         run=temp2->job_id;
                                         if(temp2->run_time==temp2->rem_time)
                                            temp2->response_time=time-temp2->arrive_time;
                                   return val2;
                                  }
				else
                                  {
                                   temp->end_time=time; 
                                   vacant=-1;
                                   return -1;
                                  }
                     
               }

          if(scheme_used==5)
            {  
              job_t* temp2=front;
		while(temp2->job_id!=job_number)
                temp2=temp2->next;
                    temp2->active=-1;
                    temp2->end_time=time;
                   temp2=front1;

                    if(front1->next==NULL)
                      { front1=front1->next;
			vacant=-1;
                        return -1;
                       }
                    else
                     { front1=front1->next;
                        if(front1->active==-1)
                 		 {  front1->active=1;
                     			temp2=front;
                        		  while(temp2->job_id!=front1->job_id)
                                		 temp2=temp2->next;
                             			 temp2->response_time=time-temp2->arrive_time;   
                  	       }   
                      return front1->job_id;
                     }

            }



        if(total_time==time)

         {    //printf("the core is idle");
              //val2= *((int *)priqueue_poll(&q));
		job_t* temp2=front;
                 tot_cores[core_id]=-1;
            while(temp2->job_id!=job_number)
              { 
               temp2=temp2->next;
              } 
                int i;
               for(i=0;i<no_cores;i++)
                  tot_cores[i]=-1;
               temp2->end_time=time;
               temp2->active=1;
              idle=time;
              vacant=-1;
              return -1;
          }
         else 
       //   printf("again");
          temp->active=1;
          run=job_number;
          return job_number;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	 //printf("quantum expired called\n");
             
              job_t *temp2=front1;
                 
                 if(temp2==NULL)
                   return -1;
               if(front1->next!=NULL)
                 {
                job_t *reset_entry=malloc(sizeof(job_t));
               // printf("gone to rechange the queue\n");
                front1=front1->next;
                reset_entry->job_id=temp2->job_id;
                reset_entry->next=NULL;
                rear1->next=reset_entry;
                rear1=reset_entry;
                if(front1->active==-1)
                  {  front1->active=1;
                     temp2=front;
                          while(temp2->job_id!=front1->job_id)
                                 temp2=temp2->next;
                              temp2->response_time=time-temp2->arrive_time;   
                  }  
                
                   
                 return front1->job_id;
                }
                else
           //       printf("repeat only one job");
                  return front1->job_id;

}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	      float count=0;
              float wait=0;
              float turn=0;
              job_t* temp2=front;
               while(temp2!=NULL)
              {   if(scheme_used==2 || scheme_used==4 || scheme_used==5)
                     {
                        turn=temp2->end_time-temp2->arrive_time;
                        wait=wait+turn-temp2->run_time;
                     }
                   else
                 wait=wait+temp2->start_time-temp2->arrive_time;
                 count++;
             //  printf(" job %d  arrival %d start %d end %d \n",temp2->job_id,temp2->arrive_time,temp2->start_time,temp2->end_time);
               temp2=temp2->next;
              }
             wait=wait/count;
             return wait;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{

              float count=0;
              float turn=0;
              job_t* temp2=front;
               while(temp2!=NULL)
              { turn=turn+temp2->end_time-temp2->arrive_time;
                 count++;
             // printf(" job %d  arrival %d start %d end %d  turn %f\n",temp2->job_id,temp2->arrive_time,temp2->start_time,temp2->end_time,turn);
               temp2=temp2->next;
              }
             turn=turn/count;

	return turn;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	 float count=0;
              float response=0;
              job_t* temp2=front;
               while(temp2!=NULL)
              {    if(scheme_used==2 || scheme_used==4 || scheme_used==5)
                        response=response+temp2->response_time;
                      else
                  response=response+temp2->start_time-temp2->arrive_time;
                 count++;
             //  printf(" job %d  arrival %d response %d end %d \n",temp2->job_id,temp2->arrive_time,temp2->response_time,temp2->end_time);
               temp2=temp2->next;
              }
             response=response/count;



           return response;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
    job_t* temp=NULL;
    temp=front;
    while(temp!=NULL){
    job_t* nex =temp->next;
    free(temp);    
    temp=nex;         
    } 
  
   priqueue_destroy(&q);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{ 
  
}
