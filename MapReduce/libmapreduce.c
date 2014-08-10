/** @file libmapreduce.c */
/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libmapreduce.h"
#include "libds/libds.h"


static const int BUFFER_SIZE = 2048;  /**< Size of the buffer used by read_from_fd(). */
pthread_t thread;
int num_process;
int epoll_fd;
int size_buf=0;
pthread_mutex_t m;
/**
 * Adds the key-value pair to the mapreduce data structure.  This may
 * require a reduce() operation.
 *
 * @param key
 *    The key of the key-value pair.  The key has been malloc()'d by
 *    read_from_fd() and must be free()'d by you at some point.
 * @param value
 *    The value of the key-value pair.  The value has been malloc()'d
 *    by read_from_fd() and must be free()'d by you at some point.
 * @param mr
 *    The pass-through mapreduce data structure (from read_from_fd()).
 */
static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{       
	// printf("key=%s value=%s  \n",key,value);
       // pthread_mutex_lock(&m);
       unsigned long ret_value; int k=0;
       ret_value=datastore_put(&mr->ds,key,value);
      // printf("ret_value is %d\n",ret_value);
       if(ret_value==0)
         {   
        const char *val=NULL;
        const char *val2=NULL;
		 unsigned long ret_get_value;
                
		 val=datastore_get(&mr->ds,key,&ret_get_value);
                 //printf("before reduce valu1=%s value2=%s revision\n",val,value);
                 val2=mr->myreduce(value,val);
                 //printf("after reduce %s \n",val);   
                 k=datastore_update(&mr->ds,key,val2,ret_get_value);
      	
	//if(val2!=NULL) {
	free(val2);
        //printf("free val2 %s\n",val2);
	val2=NULL;
	//}
        //if(val!=NULL)	{ 
	free(val);
	//printf("free val %s \n",val);
	val= NULL;     
	//}
	}
	 //if(key!=NULL)
         free(key);
         key=NULL;
 	 //if(value!=NULL){
         //printf("free value %s \n",value);
         free(value);
           value=NULL;
        //}
 	//pthread_mutex_unlock(&m);	
       //printf("key=%s value=%s revision=%d \n",key,value,revision);
}


/**
 * Helper function.  Reads up to BUFFER_SIZE from a file descriptor into a
 * buffer and calls process_key_value() when for each and every key-value
 * pair that is read from the file descriptor.
 *
 * Each key-value must be in a "Key: Value" format, identical to MP1, and
 * each pair must be terminated by a newline ('\n').
 *
 * Each unique file descriptor must have a unique buffer and the buffer
 * must be of size (BUFFER_SIZE + 1).  Therefore, if you have two
 * unique file descriptors, you must have two buffers that each have
 * been malloc()'d to size (BUFFER_SIZE + 1).
 *
 * Note that read_from_fd() makes a read() call and will block if the
 * fd does not have data ready to be read.  This function is complete
 * and does not need to be modified as part of this MP.
 *
 * @param fd
 *    File descriptor to read from.
 * @param buffer
 *    A unique buffer associated with the fd.  This buffer may have
 *    a partial key-value pair between calls to read_from_fd() and
 *    must not be modified outside the context of read_from_fd().
 * @param mr
 *    Pass-through mapreduce_t structure (to process_key_value()).
 *
 * @retval 1
 *    Data was available and was read successfully.
 * @retval 0
 *    The file descriptor fd has been closed, no more data to read.
 * @retval -1
 *    The call to read() produced an error.
 */
static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)
{      
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	if (bytes_read == 0)
		return 0;
	else if(bytes_read < 0)
	{
		fprintf(stderr, "error in read.\n");
		return -1;
	}

	buffer[offset + bytes_read] = '\0';

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
		buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}

void *thread_work(void *ptr){
 //printf("entered worker thread \n");
 mapreduce_t* mr = (mapreduce_t*)ptr;
  int counter = num_process; 
  int i;
   char s[size_buf][BUFFER_SIZE + 1];
    for(i=0;i<size_buf;i++)
          s[i][0]='\0';
    while (counter) {
       
            
         struct epoll_event ev;
           epoll_wait(epoll_fd,&ev,1,-1);
         //  printf("read data from pipe%d\n",ev.data.fd);
         
	int ret=read_from_fd(ev.data.fd, s[ev.data.fd-1], mr);
          
          if(ret==0){    --counter;
           // printf("read failed counter =%d\n",counter+1);
              //if finished read one pipe
                    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,ev.data.fd,NULL);
        }
    }
    
    /* close unused pipes */
    /* add your code here */
     /*for(i=0;i<num_process;i++)
              //close all read side of pipe
               close(fd[i][0]); 
*/
//printf("exit");

return NULL;
}


/**
 * Initialize the mapreduce data structure, given a map and a reduce
 * function pointer.
 */
void mapreduce_init(mapreduce_t *mr, 
                    void (*mymap)(int, const char *), 
                    const char *(*myreduce)(const char *, const char *))
{
	
	mr->mymap=mymap;
        mr->myreduce=myreduce;
	datastore_init(&mr->ds);	
        pthread_mutex_init(&m, NULL);
	
}


/**
 * Starts the map() processes for each value in the values array.
 * (See the MP description for full details.)
 */
void mapreduce_map_all(mapreduce_t *mr, const char **values)
{
        int n=0;
	while(values[n]!=NULL)
             n++;
        //printf("the number of child process needed are n=%d\n",n);
        int fd[n][2];
        epoll_fd=epoll_create(10);
 	struct epoll_event events[n];
        
        int i;
        for (i=0; i<n; i++) {
         pipe(fd[i]);
         pid_t pid = fork();
         int read_fd = fd[i][0];
         int write_fd = fd[i][1];
           //printf("for loop\n");
         if (pid == 0) {
            /* close unused pipes */
            //printf("executing child process\n");
             int j;
              //close all the read fd of child
              for(j=0;j<=i;j++)
                close(fd[j][0]);
               pthread_mutex_lock(&m);
               mr->mymap(fd[i][1],values[i]);
               //printf("values %s \n",values[i]);
               pthread_mutex_unlock(&m);
            close(write_fd);
            exit(0);  
          } else {
            /* close unused pipes */
            close(write_fd);
          }
         bzero(&events[i], sizeof(struct epoll_event));
         events[i].events=EPOLLIN;
         events[i].data.fd=read_fd;
         epoll_ctl(epoll_fd,EPOLL_CTL_ADD,read_fd,&events[i]);
	 size_buf=read_fd;
    }
     num_process=n;
     pthread_create(&thread,NULL,thread_work,mr);

}


/**
 * Blocks until all the reduce() operations have been completed.
 * (See the MP description for full details.)
 */
void mapreduce_reduce_all(mapreduce_t *mr)
{
  
   pthread_join(thread,NULL);
   //printf("finishhed reduce \n");
}


/**
 * Gets the current value for a key.
 * (See the MP description for full details.)
 */
const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{       //printf("called get value\n");
	char *abc=NULL;
        unsigned long ret_get_value;
	abc=datastore_get(&mr->ds,result_key,&ret_get_value);
 	//printf("returned value is %s for revision %d\n",abc,revision);
        if(abc!=NULL)
        return abc;
        else
	return NULL;
}


/**
 * Destroys the mapreduce data structure.
 */
void mapreduce_destroy(mapreduce_t *mr)
{
          datastore_destroy(&mr->ds);

}
