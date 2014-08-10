/** @file msort.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct _ptest_t {
int start;
} ptest_t;
int *text;
int cstart[1024];
int end[1024];
int values_per_segment;
int tot_elements=0;
int segment_count;

	int cmpfunc (const void * a, const void * b)
 		{
		   return ( *(int*)a - *(int*)b );
		}


  
     void *thread_sort(void *ptr)
	{ //int *result=malloc(sizeof(int));
          ptest_t* test =ptr;
          int j=0;
          int end1=test->start+values_per_segment;
          if(end1>tot_elements-1)
          end1=tot_elements;
          int size_array=end1-test->start;
         // printf("size is %d  for start %d\n",size_array,test->start);
       if(size_array>0)
         { int *arr;
            arr=malloc(sizeof(int));
          int i;
          for(i=0;i<size_array;i++)
             
          {   arr=realloc(arr,sizeof(int)*(i+1));
              arr[i]=text[test->start+i];
            // printf("%d ",arr[i]);
           }
            
           qsort(arr, size_array, sizeof(int), cmpfunc);
           //printf("start %d \n",test->start);
           for(i=test->start;i<(test->start+size_array);i++,j++)
	    text[i]=arr[j];
           
           //printf("\n");
           free(arr);
          }
          fprintf(stderr, "Sorted %d elements.\n", size_array);
         // *result=4;
           
           
         return NULL;
          
	}
     		


		void *thread_merge(void *ptr){
                //   int *result2=malloc(sizeof(int)); 
                ptest_t* test =ptr;  
                int index=0;
                int i;
                
                //printf("this is testing %d",test[2].start);
                for(i=0;i<segment_count;i++)
                   { if(test->start==cstart[i]){
                         //markindex=i;
                        // printf("if condition reached at index %d for start  %d\n",index,test->start);
                         break;}
                          index=index+1;
                   }

                 int start2;
                 int end2;
                   if(cstart[index+1]==-1){
                       
                      for(i=1;cstart[index+i]<=-1;){ i++;}
                             // printf("value of i is %d \n",i);
                              start2=cstart[index+i]; 
                              end2=end[index + i];
                         } 
                 else{
                end2=end[index+1];  
                start2=cstart[index+1];
                 }
                int end1=end[index];
                ;
                int size_array1=end1-cstart[index]+1;
                int size_array2=end2-start2+1;
                int *a1=malloc(sizeof(int));
                int *a2=malloc(sizeof(int));
                for(i=0;i<size_array1;i++)
                { a1=realloc(a1,sizeof(int)*(i+1));
		  a1[i]=text[test->start+i];}
                for(i=0;i<size_array2;i++)
                {a2=realloc(a2,sizeof(int)*(i+1));
		 a2[i]=text[start2+i]; }
                int  j, k;
                int m,n;
                m=size_array1;
                n=size_array2;
                
 		 j = k = 0;
                 int duplicate=0;
		  for (i = test->start; i < m+n+test->start;) 
                  {
                      //    printf("entered for loop for start1 %d \n",test->start);
                       
   		 	if (j < m && k < n)
                         {
      			   if (a1[j] < a2[k]) 
                             {text[i] = a1[j];j++;}
                             else 
                             {   if(a2[k]==a1[j])
                                  duplicate++;
                                text[i] = a2[k];k++;}
                              
      			i++;
    		         }
   			 else if (j == m) 
                          {
    		            for (; i < m + n+test->start;k++,i++) 
				{text[i] = a2[k];}
  	                   }
 		          else {
                                   for (; i < m+n+test->start ;j++,i++) 
                                             {text[i] = a1[j];}
                                                         
                               }         
                    }
               
              //printf("index %d start1 %d start2 %d end1 %d end2 %d\n",index,test->start,start2,end1,end2);
               // for(i=0;i<size_array1;i++)
                 //    printf("%d \n",a1[i]);
                      
           // for(i=0;i<size_array2;i++)
             //       printf(" %d \n",a2[i]);
             fprintf(stderr, "Merged %d and %d elements with %d duplicates.\n", size_array1, size_array2, duplicate);        
                    free(a1);
                    free(a2);
              //  *result2=4;
                     
                return NULL;             
		}











int main(int argc, char **argv)
{    
        
      
     
        
	 segment_count= atoi(argv[1]);	
	//printf("segment count %d \n",segment_count);
        char *line=NULL;
        size_t len=0;
        int check=0;
           
        //FILE *fp=stdin;
        text=malloc(sizeof(int));
        int i=0;
	while(getline(&line, &len, stdin) != -1)
        { if(check!=0)
          text=realloc(text,sizeof(int)*(i+1));
	  line[strlen(line)-1]='\0';
          text[i]=atoi(line);
          i++;
          check++;
          tot_elements++;
        }  
        free(line);
      // for(i=0;i<tot_elements;i++)
           //printf("%d\n",text[i]);
        
    
         /**< Maximum number of values per segment. */
	if (tot_elements % segment_count == 0)
        values_per_segment = tot_elements / segment_count;
	else
        values_per_segment = (tot_elements / segment_count) + 1;
      	//printf("values per segment are : %d\n",values_per_segment);
        pthread_t* threads=malloc(sizeof(pthread_t)*segment_count);
        ptest_t* tests =malloc(sizeof(ptest_t)*segment_count);
        int j=0;
        tests[0].start=0;
        cstart[0]=0;
        end[0]=tests[0].start+values_per_segment-1;
        for(i=0;i<segment_count-2;i++)
          {
                   tests[i+1].start=values_per_segment+j;
                   end[i+1]=tests[i+1].start-1+values_per_segment;
                   cstart[i+1]=tests[i+1].start;
                   j+=values_per_segment;
                   
          } 
         int value_lastsegment;
              if(segment_count>1)
             {
             
             value_lastsegment=tot_elements-((segment_count-1)*values_per_segment);
             tests[i+1].start=tests[i+1].start=values_per_segment+j;
             cstart[i+1]=tests[i+1].start;
             // printf("   %d \n\n",value_lastsegment);
              end[i+1]=tests[i+1].start-1+value_lastsegment;
              }
              //for(i=0;i<segment_count;i++)
       	     //printf("start %d end %d\n",cstart[i],end[i]);
        //tests[0].end =text[values_per_segment-1];
     		   for(i=0;i<segment_count;i++)
 	        {    //tests[i+1].end=tests[i+1].start+values_per_segment-1;
                      pthread_create(&threads[i],NULL,thread_sort,&tests[i]);
                 }        

                
         int *ret;
        for(i=0;i<segment_count;i++){
          pthread_join(threads[i],(void**)&ret);
            free(ret); }

      
 if(segment_count>1){
        
         int j,k;
         int *ret2;
         int new_seg=segment_count;
         j=1;int check,flag;
         pthread_t* threads2;
          while(new_seg!=1)
          {     //printf(" segment %d \n",new_seg);
               int s;
              threads2=malloc(sizeof(pthread_t)*(new_seg/2));
              flag=0;
              
              for(i=0,s=0;i<(new_seg/2);i++,s++)
               {  k=i*j*2;
                  // printf("k index is: %d and end is %d\n",k,end[k]);
                 pthread_create(&threads2[s],NULL,thread_merge,&tests[k]);
                 
               }
              for(i=0,s=0;i<(new_seg/2);i++,s++)
                {pthread_join(threads2[s],(void**)&ret2);
                    free(ret2);}
               
              for(i=0,s=0;i<(new_seg/2);i++,s++)
               { k=i*j*2;
                   int f;      
                if(tests[k+1].start==-1)
                   {   check=k;flag=-1;
                      for(f=k;i<segment_count;f++,k++)
                        { if(tests[f+2].start!=-1)
                              { k++;//printf("found");
                                break; }
                         }
                      // printf("entered if\n");
                    }
                //printf("k points at index %d\n",k);
                 tests[k+1].start=-1;
                 cstart[k+1]=-1;
                 if(flag==-1)
                 end[check]=end[k+1];
                 else
                 end[k]=end[k+1];
                 
                 }  
               free(threads2);
              if(new_seg%2!=0)
                 new_seg=(new_seg/2)+1;
              else
                 new_seg=new_seg/2;
               j=j*2;
                       
         } 

        /*  for(i=0;i<segment_count-1;i=i+2)
          {pthread_create(&threads[i],NULL,thread_merge,&tests[i]);}
     	  for(i=0;i<segment_count-1;i=i+2){        
          pthread_join(threads[i],(void**)&ret);
          tests[i+1].start=-1;
          cstart[i+1]=-1;
          end[i]=end[i+1];
          }
         

          for(i=0;i<segment_count-1;i=i+4)
       		{pthread_create(&threads[i],NULL,thread_merge,&tests[i]);
          	 pthread_join(threads[i],(void**)&ret);
           	 tests[i+2].start=-1;
                 cstart[i+2]=-1;
           	 end[i]=end[i+2];
                 }
           for(i=0;i<segment_count-1;i=i+4)
       	      {   
 		pthread_create(&threads[i],NULL,thread_merge,&tests[i]);
           	pthread_join(threads[i],(void**)&ret);
          	 tests[i+4].start=-1;
           	 cstart[i+4]=-1;
           	 end[i]=end[i+4];
       	      }
           
          
       */   
 
        		
     }// free(ret);
   
       for(i=0;i<tot_elements;i++)
       	   printf("%d\n",text[i]);
       free(text);
       free(tests);
       free(threads);
       
	return 0;
}
