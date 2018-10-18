/* TCPdaytime.c - TCPdaytime, main */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
extern int errno;
int TCPtime(const char *host,const char *service);
int errexit(const char *format, ...);
int connectTCP(const char *host, const char *service);
/*------------------------------------------------------------------------
* main - TCP client for File server
*------------------------------------------------------------------------
*/
int
main(int argc, char *argv[])
{
	char *host = "localhost";
	char *service = "5500"; /* default service port */
	if(argc!=3){
		printf("Please run with these command line inputs [host [port]]\n");
		exit(0);
	}
	switch (argc) {
		case 3:
			service = argv[2];
			/* FALL THROUGH */
		case 2:
			host = argv[1];
			break;
		default:
			fprintf(stderr, "Please run with these command line inputs [host [port ]]\n");
			exit(1);
	}
	TCPtime(host, service);
	exit(0);
}
/*------------------------------------------------------------------------
* Client - invoke on specified host and print results
*------------------------------------------------------------------------
*/
int TCPtime(const char *host, const char *service)
{
	int s1,n; /* socket, read count */
	char *buffer="hello";
	char server_buffer[2701];
	/*Client connecting to server: */
	s1 = connectTCP(host, service);
	strcpy(server_buffer,buffer);
	while(s1){
		n = write(s1,server_buffer,strlen(server_buffer));
		if (n < 0) {
			errexit("ERROR writing to socket %s\n", strerror(errno));
			}
		bzero(server_buffer,2701);
		if((n=read(s1,server_buffer,2701)) > 0){
		printf("%s\n",server_buffer);
		server_buffer[n] = '\0';
		if (server_buffer[strlen(server_buffer)-1] == '\n')
			server_buffer[strlen(server_buffer)-1] = '\0';
		if(strcmp(server_buffer,"Have a Wonderful day!!!!")==0){
			return 0;
			}
			bzero(server_buffer,2701);
			printf(":::: >>>>  ");
			fgets(server_buffer,2701,stdin);
            if(server_buffer[strlen(server_buffer) - 1] == '\n')
            server_buffer[strlen(server_buffer) - 1] = '\0';
			//printf(server_buffer);
			}
		else{ 
			printf("Error readng from the socket");
			return 0;
		}
	}
	return 0;
}

