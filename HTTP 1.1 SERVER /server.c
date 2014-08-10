/** @file server.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <queue.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "queue.h"
#include "libhttp.h"
#include "libdictionary.h"

const char *HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
const char *HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";

const char *HTTP_200_STRING = "OK";
const char *HTTP_404_STRING = "Not Found";
const char *HTTP_501_STRING = "Not Implemented";
int count=0;
pthread_t threads[4096];
dictionary_t ds;
queue_t q;
queue_t q2;
int client_fd[4096];
struct addrinfo hints, *result;
/**
 * Processes the request line of the HTTP header.
 * 
 * @param request The request line of the HTTP header.  This should be
 *                the first line of an HTTP request header and must
 *                NOT include the HTTP line terminator ("\r\n").
 *
 * @return The filename of the requested document or NULL if the
 *         request is not supported by the server.  If a filename
 *         is returned, the string must be free'd by a call to free().
 */
char* process_http_header_request(const char *request)
{
	// Ensure our request type is correct...
	if (strncmp(request, "GET ", 4) != 0)
		return NULL;

	// Ensure the function was called properly...
	assert( strstr(request, "\r") == NULL );
	assert( strstr(request, "\n") == NULL );

	// Find the length, minus "GET "(4) and " HTTP/1.1"(9)...
	int len = strlen(request) - 4 - 9;

	// Copy the filename portion to our new string...
	char *filename = malloc(len + 1);
	strncpy(filename, request + 4, len);
	filename[len] = '\0';

	// Prevent a directory attack...
	//  (You don't want someone to go to http://server:1234/../server.c to view your source code.)
	if (strstr(filename, ".."))
	{
		free(filename);
		return NULL;
	}

	return filename;
}
void handler(int sig){
	
	fflush(stdout);
	int i=0;
	for(i=0;i<count;i++)
	{
		pthread_join(threads[i],NULL);
	}
	while(queue_size(&q)!=0)
	{
    		char *data=queue_dequeue(&q);
    		free(data);
    	}
	while(queue_size(&q2)!=0)
	{
    		http_t *http=queue_dequeue(&q2);
    		http_free(http);
		free(http);
    	}
	freeaddrinfo(result); 
	exit(0);
}
void *thread_work(void *ptr){
	int *client_fd=(int*)ptr;
	char newfile[4096];
	newfile[0]='\0';
	char content_type[1000];
	content_type[0]='\0';
	const char* response_body;
	int response_code;
	char* filename;
	const char* response_code_string;
	int finish=1;
while(finish!=-1){
	http_t *http=malloc(sizeof(http_t));
	int bytes_read=-1;
	while(bytes_read==-1)
	{	
		bytes_read=http_read(http, *client_fd);
		
	}	
	printf("read %d\n",bytes_read);
	printf("status %s\n",http->status);
	printf("body %s\n",http->body);
	filename=process_http_header_request(http->status);
	const char* connection_type=NULL;
	connection_type=dictionary_get(&http->header, "Connection");
	printf("connection type =%s\n",connection_type);
	printf("filename %s\n",filename);
	if(http->body!=NULL)
	{
		int ret=dictionary_parse(&ds,http->body);
		printf("ret is %d",ret);
	}
	if(filename==NULL)
	{
		response_body=HTTP_501_CONTENT;
		response_code=501;
		response_code_string=HTTP_501_STRING;
		strcpy(content_type,"text/html");
	}
	if(filename!=NULL)
	{	response_code=200;
		response_code_string=HTTP_200_STRING;
		if(strcmp(filename,"/")==0)
		strcpy(newfile,"web/index.html");
		else
		{
		strcpy(newfile,"web");
		strcat(newfile,filename);
		}
		int access_val=access(newfile, F_OK);
        	if(access_val!=0)
		{
			response_body=HTTP_404_CONTENT;
			response_code=404;
			response_code_string=HTTP_404_STRING;
			strcpy(content_type,"text/html");
		}
	        if(access_val==0)
		{
	    	        	char * pch=NULL;
				int found=0;
		
				pch=strstr(newfile,".html");
				if(pch!=NULL && found==0)
				{
					strcpy(content_type,"text/html");
					found=1;
				}
				pch=strstr(newfile,".css");
				if(pch!=NULL && found==0)
				{
					strcpy(content_type,"text/html");
					found=1;
				}
				pch=strstr(newfile,".jpg");
				if(pch!=NULL && found==0)
				{
					strcpy(content_type,"image/jpeg");
					found=1;
				}
				pch=strstr(newfile,".png");
				if(pch!=NULL && found==0)
				{
					strcpy(content_type,"image/png");
					found=1;
				}
				if(found==0)
				strcpy(content_type,"text/plain");
	   	}// if the file exist on disk		
	}// if filename!=null
	int content_length=0;
	struct stat st;	
	switch(response_code)
	{
		case 404: 	content_length=strlen(HTTP_404_CONTENT);
				break;
		case 501:	content_length=strlen(HTTP_501_CONTENT);	
				break;
		case 200:	stat(newfile, &st);
                    		content_length = st.st_size;
				break;
	}

	char response_line[1000];
	response_line[0]='\0';
	char *buffer_send=malloc(content_length+1000);
	sprintf(response_line, "HTTP/1.1 %d %s\r\n", response_code, response_code_string);
	
	strcpy(buffer_send,response_line);
	sprintf(response_line, "Content-Type: %s\r\n",content_type);
        strcat(buffer_send,response_line);
	sprintf(response_line, "Content-Length: %d\r\n",content_length );
        strcat(buffer_send,response_line);
	if(connection_type!=NULL)
	{
		if(strcasecmp(connection_type,"Keep-Alive")==0)
			{	strcat(buffer_send,"Connection: Keep-Alive\r\n");
				finish=-1;
			}
		else
			strcat(buffer_send,"Connection: close\r\n");
	}
	else
		strcat(buffer_send,"Connection: close\r\n");
	strcat(buffer_send,"\r\n");	
	printf("buffer_send =\n%s",buffer_send);
	int buffer_send_len=strlen(buffer_send);
	int read=0;
	FILE *fp;
	switch(response_code)
	{
		case 404: 	strcat(buffer_send,HTTP_404_CONTENT);
                        	buffer_send_len= strlen(buffer_send);

				break;
		case 501:	strcat(buffer_send,HTTP_501_CONTENT);
                        	buffer_send_len= strlen(buffer_send);	
				break;
		case 200:	fp = fopen(newfile,"rb");
				while((read= fread(buffer_send+buffer_send_len,1, content_length,fp))>0 )
                        	{
                                	buffer_send_len += read;
                        	}
				fclose(fp);

				break;
	}
	//printf("buffer_send =\n%s",buffer_send);
	send(*client_fd, buffer_send,buffer_send_len, 0);
	queue_enqueue(&q, buffer_send);
	queue_enqueue(&q2, http);
        queue_enqueue(&q, filename);
	//free(buffer_send);
	//http_free(http);
	//free(filename);
	//free(http);
	
   }
	close(*client_fd);
	pthread_exit(NULL);	
}
int main(int argc, char **argv)
{
	dictionary_init(&ds);
	queue_init(&q);
	queue_init(&q2);
        char *port_number;
        port_number=argv[1];
        int sock_fd=socket(AF_INET,SOCK_STREAM,0);
	
	memset(&hints,0x00,sizeof(struct addrinfo));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	getaddrinfo(NULL,port_number,&hints,&result);
	bind(sock_fd,result->ai_addr,result->ai_addrlen);
	listen(sock_fd,10);
	
	
	signal(SIGINT,handler);
	while((client_fd[count]=accept(sock_fd,NULL,NULL))){
	pthread_create(&threads[count],NULL,thread_work,&client_fd[count]);
		count++;
	}

	return 0;
}
