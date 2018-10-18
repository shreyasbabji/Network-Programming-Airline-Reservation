/* TCPechod.c - main, TCPechod */
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sqlite3.h>

#define QLEN 5 /* maximum connection queue length */
#define BUFSIZE 4096
extern int errno;
sem_t sock_lock;
void system_admin(void *ssock);
void flight(void* sock);
void customer(void* sock);
int login_(char* credentials,char* User);
void execute_command_db();
void AirlineMain(void* sock);
void Customer_funct(void* sock);
void reaper(int);
void *Server(void *vargp);
int errexit(const char *format, ...);
int passiveTCP(const char *service, int qlen);
static int callback_view_users(void *data, int argc, char **argv, char **azColName);
int pass_match=0;
char db_call_result[1500];

sqlite3 *db;
sqlite3_stmt *stmt;
int rc;
char* query;
/*------------------------------------------------------------------------
* main - Concurrent TCP server for ECHO service
*------------------------------------------------------------------------
*/
int msock; /* master server socket */
int
main(int argc, char *argv[])
{
	char *service = "5500"; /* service name or port number */
	pthread_t tid;
	switch (argc) {
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("usage: TCPechod [port]\n");
	}
	msock = passiveTCP(service, QLEN);
	//printf("%d\n",msock);
		/*
	* open SQLite database file test.db
	* use ":memory:" to use an in-memory database
	*/
	rc = sqlite3_open("test.db", &db);
	if (rc != SQLITE_OK) {
	printf("ERROR opening SQLite DB 'test': %s\n", sqlite3_errmsg(db));
	}
	char* name = "System_Admin";
	char* password = "iamadmin";

	//asprintf(&query, "create table Flight_Users(name text PRIMARY KEY not null, password text not null);");
	//execute_command_db();
   // asprintf(&query, "insert into Flight_Users (name, password) values ('%s', '%s');",name,password);
	//execute_command_db();
	while(1){
		pthread_create(&tid, NULL, Server, (void*)msock);				
		pthread_join(tid, NULL);
    }
   // sqlite3_close(db);
    pthread_exit(NULL);
}
/*------------------------------------------------------------------------
* TCPechod - echo data until end of file
*------------------------------------------------------------------------
*/
void
*Server(void *vargp)
{
	int fd = (int) vargp;	
	struct sockaddr_in fsin; /* the address of a client */
	socklen_t alen; /* length of client's address */
	char client_buffer[1001];
	char* user_sa="1";
	char* user_fa="2";
	char* user_cust="3";
	char *Welcome_screen = "Hi! Welcome to Airline reservation system. Please enter the user functionality you would like to access today.\n1. System Admin 2. Flight services 3. Customer\n";
	int n,n1;
	int ssock; /* slave server socket */
	alen = sizeof(fsin);
	printf("thread executing\n");
	ssock = accept(fd, (struct sockaddr *)&fsin, &alen);
	if (ssock < 0) {
			if (errno == EINTR)
			errexit("accept: %s\n", strerror(errno));
		}
	bzero(client_buffer,1001);
	/*Reading value from the client*/
	n = read(ssock,client_buffer,1001);
	if (n < 0) {
		errexit("ERROR reading from socket %s\n", strerror(errno));
		}
	bzero(client_buffer,1001);
	printf("writing to the clilent\n");
	strcpy(client_buffer,Welcome_screen);
	n1=write(ssock,client_buffer,strlen(client_buffer));
	if (n1 <= 0){
		errexit("Server:ERROR writing to socket\n");
		}
	printf("reading from the client\n");
	bzero(client_buffer,1001);
	if((n = read(ssock,client_buffer,1001)>0)){
		if(strcmp(client_buffer,user_sa)==0){
		system_admin((void*)ssock);
		}
		if(strcmp(client_buffer,user_fa)==0){
		flight((void*)ssock);
		}
		if(strcmp(client_buffer,user_cust)==0){
		customer((void*)ssock);
        }
    }
	else{
		printf("Error readng from the socket\n");
		}

	(void) close(ssock);
    return NULL;
}

void system_admin(void* sock){
	int ssock= (int) sock;
	int n1;
	char client_buffer[1001];
	char *Welcome_screen_sa = "Welcome System Admin. Please login. Enter <Username> <Password>";
	char *Succ_Login = "Logged in Successfully\n";
	char *Failure_Login = "Incorrect Username or Password\n";
	char *Sys_admin_options= "1. Add User 2. Modify User 3.View Users 4.Delete a User 5.Exit\n";
	char *add_user= "Please enter user details to add a user: <Username> <Password>:";
	char *not_implemented = "Not Implemented!!!\n";
	//char *modify_user= "Please enter old password: <Password>:";
	char *modify_user= "Please enter new password: <Username> <Password>:";
//	strcat(add_user,"Enter in the below format:\n");
//	strcat(add_user,"<User Name> <Password>");
again:
	bzero(client_buffer,1001);
	printf("writing to the clilent\n");
	strcpy(client_buffer,Welcome_screen_sa);
	n1=write(ssock,client_buffer,strlen(client_buffer));
	if (n1 <= 0){
		errexit("Server:ERROR writing to socket\n");
		}
	bzero(client_buffer,1001);
	/*Reading value from the client*/
	n1 = read(ssock,client_buffer,1001);
	if (n1 < 0) {
		errexit("ERROR reading from socket %s\n", strerror(errno));
		}
	if(login_(client_buffer,"System_Admin")==1){
		bzero(client_buffer,1001);
		printf("writing to the clilent\n");
		strcpy(client_buffer,Succ_Login);
		n1=write(ssock,client_buffer,strlen(client_buffer));
		pass_match = NULL;
	}
	else {
		bzero(client_buffer,1001);
		printf("writing to the clilent\n");
		strcpy(client_buffer,Failure_Login);
		n1=write(ssock,client_buffer,strlen(client_buffer));
		goto again;	
	}
	if (n1 <= 0){
		errexit("Server:ERROR writing to socket\n");
		}
while(1){
		bzero(client_buffer,1001);
		printf("writing to the clilent\n");
		strcpy(client_buffer,Sys_admin_options);
		n1=write(ssock,client_buffer,strlen(client_buffer));
		if (n1 <= 0){
			errexit("Server:ERROR writing to socket\n");
			}
		bzero(client_buffer,1001);
		n1 = read(ssock,client_buffer,1001);
		if (n1 < 0) {
			errexit("ERROR reading from socket %s\n", strerror(errno));
			}
		printf("%s\n",client_buffer);
		if(strcmp(client_buffer,"1")==0){
			bzero(client_buffer,1001);
			printf("writing to the clilent\n");
			strcpy(client_buffer,add_user);
			n1=write(ssock,client_buffer,strlen(client_buffer));
			if (n1 <= 0){
				errexit("Server:ERROR writing to socket\n");
				}
			bzero(client_buffer,1001);
			n1 = read(ssock,client_buffer,1001);
			if (n1 < 0) {
				errexit("ERROR reading from socket %s\n", strerror(errno));
				}
			char* name=NULL;
			char* password=NULL;
			name = strtok(client_buffer, " ");
			int i=0;
			while(i<1) 
			{
				password = strtok(NULL, " ");
				i++;
			}
			asprintf(&query, "insert into Flight_Users (name, password) values ('%s', '%s');",name ,password);
			execute_command_db();
		}
		if(strcmp(client_buffer,"2")==0){
			bzero(client_buffer,1001);
			printf("writing to the clilent\n");
			strcpy(client_buffer,modify_user);
			n1=write(ssock,client_buffer,strlen(client_buffer));
			if (n1 <= 0){
				errexit("Server:ERROR writing to socket\n");
				}
			bzero(client_buffer,1001);
			n1 = read(ssock,client_buffer,1001);
			if (n1 < 0) {
				errexit("ERROR reading from socket %s\n", strerror(errno));
				}

			char* name=NULL;
			char* password=NULL;
			name = strtok(client_buffer, " ");
			int i=0;
			while(i<1) 
			{
				password = strtok(NULL, " ");
				i++;
			}

			asprintf(&query, "Update Flight_Users set password = '%s' where name = '%s';",password ,name);
			execute_command_db();
		}
		if(strcmp(client_buffer,"3")==0){
			bzero(client_buffer,1001);
			char* sql;
			char* errmsg;
		
			sql = "SELECT name from Flight_Users;";
			rc = sqlite3_exec(db, sql, callback_view_users, NULL, &errmsg);
			if (rc != SQLITE_OK) {
				//fprintf(stderr, "SQL error: %s\n", errmsg);
				sqlite3_free(errmsg);
				//fprintf(stdout, "password %s info retrieved successfully\n",uname);
			}
			bzero(client_buffer,1001);
			strcpy(client_buffer,"User of this system\n");
			strcat(client_buffer,db_call_result);
			strcat(client_buffer,"What would you like to do next\n");
			sqlite3_finalize(stmt);
			printf("writing to the client\n");
			n1=write(ssock,client_buffer,strlen(client_buffer));
			strcpy(db_call_result,"");
			if (n1 <= 0){
				errexit("Server:ERROR writing to socket\n");
				}
		}
		if(strcmp(client_buffer,"4")==0){
			bzero(client_buffer,1001);
			printf("writing to the clilent\n");
			strcpy(client_buffer,"Please enter the name of user to be deleted:<User Name>");
			n1=write(ssock,client_buffer,strlen(client_buffer));
			if (n1 <= 0){
				errexit("Server:ERROR writing to socket\n");
				}
			bzero(client_buffer,1001);
			n1 = read(ssock,client_buffer,1001);
			if (n1 < 0) {
				errexit("ERROR reading from socket %s\n", strerror(errno));
				}		
			asprintf(&query, "Delete from Flight_Users where name = '%s';",client_buffer);
			execute_command_db();
			printf("writing to the clilent\n");
			bzero(client_buffer,1001);
			strcpy(client_buffer,db_call_result);
			n1=write(ssock,client_buffer,strlen(client_buffer));
			strcpy(db_call_result,"");
			if (n1 <= 0){
				errexit("Server:ERROR writing to socket\n");
				}

		}
		if(strcmp(client_buffer,"5")==0){
					bzero(client_buffer,1001);
					printf("writing to the clilent\n");
					strcpy(client_buffer,"Have a Wonderfull day!!!!\n");
					n1=write(ssock,client_buffer,strlen(client_buffer));
					if (n1 <= 0){
						errexit("Server:ERROR writing to socket\n");
						}
					bzero(client_buffer,1001);
					return;
				}
	}
}



void flight(void* sock){
	int ssock= (int) sock;
	int n1;
	char client_buffer[1001];
	char *Welcome_screen_fa = "Welcome Flight Professionals. Please login. Enter <username> <password>";
	char *Succ_Login = "Logged in Successfully\n";
	char *Failure_Login = "Incorrect Username or Password\n";
	//char *FA_options= "Not implemented!!\n";
again:
	bzero(client_buffer,1001);
	printf("writing to the client\n");
	strcpy(client_buffer,Welcome_screen_fa);
	n1=write(ssock,client_buffer,strlen(client_buffer));
	if (n1 <= 0){
		errexit("Server:ERROR writing to socket\n");
		}
	bzero(client_buffer,1001);
	/*Reading value from the client*/
	n1 = read(ssock,client_buffer,1001);
	if (n1 < 0) {
		errexit("ERROR reading from socket %s\n", strerror(errno));
		}
	if(login_(client_buffer,"Flight")==1){
		bzero(client_buffer,1001);
		printf("writing to the client\n");
		strcpy(client_buffer,Succ_Login);
		n1=write(ssock,client_buffer,strlen(client_buffer));
		pass_match = NULL;
	}
	else {
		bzero(client_buffer,1001);
		printf("writing to the client\n");
		strcpy(client_buffer,Failure_Login);
		n1=write(ssock,client_buffer,strlen(client_buffer));
		goto again;	
	}
	if (n1 <= 0){
		errexit("Server:ERROR writing to socket\n");
		}
    sqlite3_close(db);
    AirlineMain((void*)ssock);
    rc = sqlite3_open("test.db", &db);
    if (rc != SQLITE_OK) {
        printf("ERROR opening SQLite DB 'test': %s\n", sqlite3_errmsg(db));
    }

}


void customer(void* sock){
	int ssock= (int) sock;
	//int n1;
	//char client_buffer[1001];
	//char *Welcome_screen_cust = "Welcome to Airline reservation. Are you registered with us?\n 1. New User 2. Existing User ";
	//char *FA_options= "Not implemented!!\n";
again:
	//bzero(client_buffer,1001);
	//printf("writing to the clilent\n");
	//strcpy(client_buffer,Welcome_screen_cust);
//	n1=write(ssock,client_buffer,strlen(client_buffer));
//	if (n1 <= 0){
//		errexit("Server:ERROR writing to socket\n");
	//	}
    sqlite3_close(db);
    Customer_funct((void*)ssock);
    rc = sqlite3_open("test.db", &db);
    if (rc != SQLITE_OK) {
        printf("ERROR opening SQLite DB 'test': %s\n", sqlite3_errmsg(db));
    }


}


int login_(char* credentials,char* User){
	char* token=NULL;
	char* password=NULL;
	token = strtok(credentials, " ");
	int i=0;
	while( token != NULL && i<1) 
	{
		password = strtok(NULL, " ");
		i++;
	}
	if(strcmp(User,"System_Admin ")==0){
		if(!(strcmp(token,"System_Admin")))
			return 0;
	}
	if(password!=NULL){
		return verify(token,password);}
	else return 0;
}
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
	pass_match=0;
   // fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i<argc; i++){
       // printf("%d = %s\n",i,argv[i]);
	if(strcmp((const char*) data,argv[i])==0){	
	pass_match=1;
	}
    }
    printf("\n");
    return 0;
}


static int callback_view_users(void *data, int argc, char **argv, char **azColName){
    int i;
   // fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i<argc; i++){
        strcat(db_call_result,argv[i]);
	strcat(db_call_result,"\n");
	}
    return 0;
}

int verify(char* uname,char* password){
	char *errmsg;
	char *sql;
	char buffer[256];
	//rc = sqlite3_exec(db,"select password from Flight_Users where name='?';",callback, NULL, &errmsg);
	strcpy(buffer, "SELECT password from Flight_Users where name='");
	strcat(buffer, uname);
	strcat(buffer, "';");
	sql = buffer;                                                             /* 1 */
	rc = sqlite3_exec(db, sql, callback, (void*) password, &errmsg);
	
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        fprintf(stdout, "password %s info retrieved successfully\n",uname);}
    
	sqlite3_finalize(stmt);
	return pass_match;
}

void execute_command_db(){

	/*
	* execute commands that do not return results
	*/
	sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
	}
	else{
		strcpy(db_call_result,"User was deleted Successfully\n");
	}
	sqlite3_finalize(stmt);
	free(query);
}
