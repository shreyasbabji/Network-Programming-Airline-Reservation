//
//  main.c
//  tcpserver_airline_project
//
//  Created by Ifunanya Nnoka on 10/28/16.
//  Copyright © 2016 Ifunanya Nnoka. All rights reserved.
//


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
void AirlineMain(void* sock);
int errexit(const char *format, ...);
char * extract_line(char *field,const char filename[],int no_of_bytes);
char * cmpr_airline(char *fieeld,int no_bytes);
char *zErrMsg = 0;
void create_table(void);
void create_UID_store(void);
int view_mod_exec (void);
int view_flight (const char *name);
int view_airline (const char *airline);
void add_col(void);
int view_all_flights(void);
int add_flight(const char *flight);
int uid_confirm (const char *airline,const char *id);
void freebuf(char **buf);
//int sqlite3_key(sqlite3 *db2,const void *pKey,int nKey);
void get_uid(void);
void add_uid (void);
int modify_flight(const char *flight);
int delete_flight(const char *name);
char buffer[256];
int id_ok,ok_;
int a_ok;
char call_buff[2701];
char *cb = call_buff;
sqlite3 *db2;
int rc2;
//const char* data = "Callback function called";

void AirlineMain(void* sock)
    {
    char fromclient[2701];
    char toclient[2701];
    char flight_option;
    //FILE *source;
    int n1;
    int j = 0;
    int true = 1;
    int ssock = (int) sock; /* slave server socket */
    char UID[50];
    char name[50];
    char airline_[50];
    char f_source[50];
    char destination[50];
    char price[50];
    char total_seats[50];
    char open_seats[50];
    char date_depart[50];
    char date_arrive[50];
    char *flight_service_options = "1. Add Flight 2. Search a Flight 3. "\
        "Modify Flight 4. Delete Flight\n 5. View All Available Flights 6. "\
        "View All Flights from a specific airline 7. Exit\n";
    /*Open Database*/
    rc2 = sqlite3_open("airline.db", &db2);
    if( rc2 ){
        fprintf(stderr, "Database failed to open: %s\n", sqlite3_errmsg(db2));
    }
    else
        fprintf(stderr, "Database opened successfully\n");
      
    // create_table();
    // create_UID_store();
   // sqlite3_key(db2,12345678, 8);
      //  add_col();
    while (true) {
    bzero(toclient,sizeof(toclient));
    printf("writing to the client\n");
    strcpy(toclient,flight_service_options);
    n1=write(ssock,toclient,strlen(toclient));
    if (n1 <= 0){
        errexit("ERROR writing to socket\n");
    }
    bzero(fromclient,sizeof(fromclient));
    n1 = read(ssock,fromclient,sizeof(fromclient));
    if (n1 < 0) {
        errexit("ERROR reading from socket %s\n", strerror(errno));
    }
        flight_option = fromclient[0];
        switch (flight_option) {
            case '1':
                printf("Request from client to add flight\n");
        add:    /*Request for  name of airline*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Airline name: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(airline_,fromclient);
                /*Request for airline ID*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Airline ID: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(UID,fromclient);
                int p = 0;
                p = uid_confirm (airline_,UID);
                if (p == 0) {
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient,"UID %s already assigned to a different airline.\n"\
                            "Enter y to use a different ID, n to exit!\n",UID);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                        goto add;
                    }else break;
                }
                else if (p == 3){
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient,"Incorrect ID entered for airline %s.\n"\
                            "Enter y to use a different ID, n to exit!\n",airline_);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                        goto add;
                    }else break;
                }
                else if (p == 2){
                    bzero(buffer, sizeof(buffer));
                    sprintf(buffer, "insert into UID_store values (%s, '%s');",UID,airline_);
                    add_uid ();
                }
                /*Request for flight name*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Flight name: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(name,fromclient);                
                /*Request for flight source*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Flight Source: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(f_source,fromclient);
                /*Request for flight destination*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Flight Destination: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(destination,fromclient);
                /*Request for flight price*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Flight Price: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(price,fromclient);
                /*Request for total seats in flight*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Total Number of Seats: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(total_seats,fromclient);
                /*Request for number of open seats in flight*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Number of Seats Available: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(open_seats,fromclient);
                /*Request for total seats in flight*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter date and time of Depature: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(date_depart,fromclient);
                /*Request for number of open seats in flight*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter date and time of arrival: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(date_arrive,fromclient);
                bzero(buffer, sizeof(buffer));
                sprintf(buffer, "insert into DATA values ('%s', %s, '%s', '%s',"\
                "'%s', %s, %s, %s,'%s','%s');",name ,UID,airline_,
                f_source,destination,price,total_seats,open_seats,date_depart,date_arrive);
                bzero(call_buff, 2001);
                p = add_flight(name);
                if (p == 1) {
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient,"\nFlight %s added to server!\n\nFlight Summary:\n\n%s\n"\
                            "Enter y to add more flights, n to exit.",name,call_buff);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                else
                {   printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient,"Undefined Error.Unable to add %s to database.\n"\
                            "Enter y to add more flights, n to exit.",name);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                    }
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                    goto add;
                }
                else break;
            case '2':
        search: printf("Request from client to view a flight\n");
                /*Request for  name of flight*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Flight Name: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(name,fromclient);
                int i = 0;
                bzero(call_buff, 2001);
                i = view_flight(name);
                if(i != 1){
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient, "\nWe're sorry that flight does not exist on our server."\
                            "\nEnter y to search more fields, n to exit.");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                else{
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient, "\nSearch Result:\n\n%s\n"\
                    "Enter y to search more flights, n to exit",call_buff);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                    goto search;
                }
                break;
            case '3':
            modify: printf("Request from client to modify flight\n");
                    /*Request for  name of flight*/
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"Enter Flight Name: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(name,fromclient);
                    int o = 0;
                    o = view_flight(name);
                    if(o != 1){
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"\nWe're sorry that flight does not exist on our server.\n");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                        goto modify;
                    }
                    bzero(buffer, sizeof(buffer));
                    sprintf(buffer, "SELECT UID from DATA where name = '%s';",name);
                    get_uid();
                    strcpy(UID, call_buff);
            more:   printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient,"\nRequest to modify flight %s with airline ID %s.\n"\
                            "Enter to modify:\n<<:: 1. All ::>> <<:: 2. Source ::>> <<:: 3."\
                            "Destination ::>> <<:: 4. Price ::>>\n<<:: 5. No of Seats Available ::>>"\
                            "<<:: 6. Date of Departure ::>> <<:: 7. Date of Arrival ::>>",name,UID);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    if (fromclient[0] == '1') {
                        j = 1;
                    }
                    int a = 1;
                    bzero(buffer, sizeof(buffer));
                    if((j == 1)||((fromclient[0] == '2')&&(a == 1))){
                    /*Request for flight source*/
                    a =0;
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"Enter Flight Source: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(f_source,fromclient);
                    sprintf(buffer, "UPDATE DATA set SOURCE = '%s' where name = '%s'",f_source, name);
                    }
                    if((j == 1)||((fromclient[0] == '3')&&(a == 1))){
                    /*Request for flight destination*/
                    a = 0;
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"Enter Flight Destination: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(destination,fromclient);
                    sprintf(buffer, "UPDATE DATA set DESTINATION = '%s' where name = '%s'",destination, name);
                    }
                    if((j == 1)||((fromclient[0] == '4')&&(a == 1))){
                    /*Request for flight price*/
                    a = 0;
                    printf("writing to the client\n");
                    bzero(toclient,2001);
                    strcpy(toclient,"Enter Flight Price: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(price,fromclient);
                    sprintf(buffer, "UPDATE DATA set PRICE = %s where name = '%s'",price, name);
                    }
                    if((j == 1)||((fromclient[0] == '5')&&(a == 1))){
                    /*Request for number of open seats in flight*/
                    a = 0;
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"Enter Number of Seats Available: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(open_seats,fromclient);
                    sprintf(buffer, "UPDATE DATA set OPEN_SEATS = %s where name = '%s'",open_seats, name);
                    }
                    if((j == 1)||((fromclient[0] == '6')&&(a == 1))){
                    /*Request for Departure date/time*/
                    a = 0;
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"Enter Date/Time of Departure: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(date_depart,fromclient);
                    sprintf(buffer, "UPDATE DATA set DATE_DEPATURE = '%s' where name = '%s'",date_depart, name);
                    }
                    if((j == 1)||((fromclient[0] == '7')&&(a == 1))){
                    /*Request for Date/Time of Arrival*/
                    a = 0;
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"Enter Date/Time of Departure: ");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    strcpy(date_arrive,fromclient);
                    sprintf(buffer, "UPDATE DATA set DATE_ARRIVAL = '%s' where name = '%s'",date_arrive, name);
                    }
                    if (j == 1) {
                        bzero(buffer, sizeof(buffer));
                        sprintf(buffer, "UPDATE DATA set SOURCE = '%s',DESTINATION = '%s',"\
                        "PRICE = %s, OPEN_SEATS = %s,DATE_DEPATURE = '%s',DATE_ARRIVAL = '%s' where name = '%s'",
                        f_source, destination, price, open_seats,date_depart,date_arrive, name);
                        j = 0;
                    }
                    bzero(call_buff, 2001);
                    int y = modify_flight(name);
                    if (y != 1) {
                        printf("writing to the client\n");
                        bzero(toclient,sizeof(toclient));
                        sprintf(toclient, "\nSQL error: %s\nEnter y to modify more fields, n to exit.", zErrMsg);
                        n1=write(ssock,toclient,strlen(toclient));
                        if (n1 <= 0){
                            errexit("ERROR writing to socket\n");
                        }
                    }else{
                        printf("writing to the client\n");
                        bzero(toclient,sizeof(toclient));
                        sprintf(toclient,"Updated Flight Details for %s:\n\n%s\n"\
                                "Enter y to modify more fields, n to exit",name,call_buff);
                        n1=write(ssock,toclient,strlen(toclient));
                        if (n1 <= 0){
                            errexit("ERROR writing to socket\n");
                        }
                    }
                    printf("reading from the client\n");
                    bzero(fromclient,sizeof(fromclient));
                    n1 = read(ssock,fromclient,sizeof(fromclient));
                    if (n1 < 0) {
                        errexit("ERROR reading from socket %s\n", strerror(errno));
                    }
                    if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                        goto more;
                    }
                    break;
            case '4':
                printf("Request from client to delete flight\n");
                /*Request for  name of flight*/
        delete: printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Flight Name: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(name,fromclient);
                int d = 0;
                bzero(call_buff, 2001);
                d = view_flight(name);
                if(d!= 1){
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"\nWe're sorry that flight does not exist on our server.");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    goto delete;
                }
                d=0;
                bzero(call_buff, 2001);
                d = delete_flight(name);
                if (d != 1){
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                sprintf(toclient, "\nSQL error: %s\n"\
                "Enter y to delete another field, n to exit.", zErrMsg);
                }
                else{
                    printf("writing to the client\n");
                    bzero(toclient,2001);
                    sprintf(toclient,"Flight %s deleted successfully!\n"\
                    "Enter y to delete more flights, n to exit.",name);
                }
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                    goto delete;
                }

                break;
            case '5':
                printf("Request from client to view all available flights \n");
                bzero(call_buff, 2001);
                int j = view_all_flights();
                if( j != 1){
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    strcpy(toclient,"No flights available at the moment. Please try again later!");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                else
                {
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient, "\nSearch Results: All Flights\n\n%s",call_buff);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                break;
            case '6':
search_airline: printf("Request from client to view all flights of an airline \n");
                /*Request for  name of airline*/
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Enter Airline name: ");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                strcpy(airline_,fromclient);
                /* Create select SQL statement */
               // sprintf(buffer, "SELECT * from DATA where airline = '%s';",airline_);
                bzero(call_buff, 2001);
                int m = view_airline(airline_);
                if(m != 1){
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    sprintf(toclient, "\nWe're sorry there are no records of flights from %s on our server.\n"\
                            "Enter y to search a different airline, n to exit.", airline_);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                else{
                    printf("writing to the client\n");
                    bzero(toclient,sizeof(toclient));
                    //strcpy(toclient, call_buff);
                    sprintf(toclient, "\nSearch Result: all for airline %s\n\n%s\n"\
                    "Enter y to search a different airline, n to exit.",airline_,call_buff);
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                }
                printf("reading from the client\n");
                bzero(fromclient,sizeof(fromclient));
                n1 = read(ssock,fromclient,sizeof(fromclient));
                if (n1 < 0) {
                    errexit("ERROR reading from socket %s\n", strerror(errno));
                }
                if ((fromclient[0] == 'y')||(fromclient[0] == 'Y')) {
                    goto search_airline;
                }
                break;
            case '7':
                printf("writing to the client\n");
                bzero(toclient,sizeof(toclient));
                strcpy(toclient,"Have a Wonderfull day!!!!");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                true = 0;
                break;
            default:
                errexit("Invalid request\n");
                break;
        }
    }
   sqlite3_close(db2);
  //  (void) close(ssock);
}


static int callback(void *data, int argc, char **argv, char **azColName){
   // char *cb = call_buff;
    int i;
    for(i=0; i<argc; i++){
        cb += sprintf(cb,"%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if (argv[i] == NULL) {
            ok_ = 0;
        }else ok_ = 1;
    }
    cb += sprintf(cb,"\n");
    return 0;
}

static int callback2(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        sprintf(call_buff,"%s\n",argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

static int callback4(void *data, int argc, char **argv, char **azColName)
{
    int i;
    FILE *fp;
    int stdout_bk; //fd for stdout backup
    stdout_bk = dup(fileno(stdout));
    fp = fopen(data, "a");
    dup2(fileno(fp), fileno(stdout));
    for (i=0; i<argc; i++) {
        printf("%s",argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    dup2(stdout_bk, fileno(stdout)); //restore
    fclose(fp);
    return 0;
}
void create_table(void)
{
    char *sql;
    /*Creates SQL statement*/
    sql = "Create table data("\
    "NAME char primary key not null,"\
    "UID             int not null,"\
    "AIRLINE         text not null,"\
    "SOURCE          text not null,"\
    "DESTINATION     text not null,"\
    "PRICE           real,"\
    "TOTAL_SEATS     int not null,"\
    "OPEN_SEATS      int not null,"\
    "DATE_DEPATURE   char not null,"\
    "DATE_ARRIVAL    char not null);";
    
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
     //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
}
void add_col(void)
{
    char *sql;
    sql ="ALTER TABLE data ADD COLUMN DATE_DEPATURE char;";
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
     //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sql ="ALTER TABLE data ADD COLUMN DATE_ARRIVAL char;";
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
     //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

}

void create_UID_store(void)
{
    char *sql;
    /*Creates SQL statement*/
    sql = "Create table UID_STORE("\
    "UID int primary key not null,"\
    "AIRLINE         text not null);";
    
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
      //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
}

void add_uid (void)
{
    /*Creates SQL statement. Inserts information into SQL table named "uid_store"*/
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "New airline and UID added to database\n");
    }
 //   free(buffer);
}

int uid_confirm (const char *airline,const char *id)
{
    FILE *source;
    char store[50];
    char *sql;
    const char filename[] = "UIDSTORE.txt";
    sql = "SELECT * from UID_STORE;";
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback4, (void*)filename, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    char *buff1 = extract_line((char*)id,filename,3);
    bzero(store, 50);
    strncpy(store, id,3);
    strcat(store, airline);
    id_ok = 0;
    /*If UID matches airline*/
    if(strncmp(store, buff1, strlen(store))== 0){
        id_ok = 1;
    }else if(*buff1 == '\0'){
        char *buff3 = cmpr_airline(airline,strlen(airline));
        if (*buff3 == '\0') {
            id_ok = 2;
        }
        else
            id_ok = 3;
    }
    else id_ok = 0;
    source = fopen(filename, "w");
    fprintf(source, "");
    fclose(source);
    return id_ok;
}



void get_uid(void)
{
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback2,0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

int add_flight(const char *flight)
{
    cb = &call_buff[0];
    a_ok = 0;
    /*Creates SQL statement. Inserts information into SQL table named "data"*/
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        a_ok =1;
    }
    cb = &call_buff[0];
    bzero(buffer, sizeof(buffer));
    /*Call back added flight*/
    sprintf(buffer, "SELECT * from DATA where NAME = '%s';",flight);
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return a_ok;
}
int view_all_flights(void)
{
    char *sql;
    int i =0;
    //bzero(call_buff, 2001);
    cb = &call_buff[0];
    sql = "SELECT * from DATA;";
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback,0, &zErrMsg);
    
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        i=1;
    }
    return i;
}

int view_mod_exec (void)
{
    ok_ = 0;
    cb = &call_buff[0];
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback,0, &zErrMsg);
    
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return ok_;
}




int view_airline (const char *airline)
{
    char buff[256];
    char *sql;
    ok_ = 0;
    cb = &call_buff[0];
    bzero(buff, 256);
    /*Execute SQL statement*/
    sprintf(buff, "SELECT * from DATA where airline = '%s';",airline);
    sql = buff;
    rc2 = sqlite3_exec(db2, sql, callback,0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return ok_;
}

int view_flight (const char *name)
{
    ok_ = 0;
    cb = &call_buff[0];
    bzero(buffer, sizeof(buffer));
    sprintf(buffer, "SELECT * from DATA where name = '%s';",name);
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback,0, &zErrMsg);
    
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return ok_;
}

int modify_flight(const char *flight)
{
    int i = 0;
    char buff[256];
    char *sql;
    cb = &call_buff[0];
    /* Create UPDATE SQL statement */
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, buffer, callback,0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        i=1;
    }
    cb = &call_buff[0];
    bzero(buff, 256);
    /*Call back modified flight*/
    sprintf(buff,"SELECT * from DATA where NAME = '%s';",flight);
    sql = buff;
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return i;

}


int delete_flight(const char *name)
{
    int n = 0;
    char *sql;
    char buff[256];
    bzero(buff, 256);
    /* Create DELETE SQL statement */
    sprintf(buff,"DELETE from DATA where NAME = '%s';",name);
    sql = buff;
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //fprintf(stdout, "Flight %s info deleted successfully\n",name);
        n = 1;
    }
    cb = &call_buff[0];
    bzero(buff, 256);
    strcpy(buff, "SELECT * from DATA");
    sql = buff;
    /*Execute SQL statement*/
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return n;
}


char * extract_line(char *field,const char filename[],int no_of_bytes)
{
    // static const char filename[] = "view_flight.txt";
    char line_temp[50];
    char *line =  (char *)malloc(sizeof(char) * 50);
    FILE *pfile = fopen(filename,"r");
    if (pfile != NULL)
    {
        while (fgets(line_temp, sizeof(line_temp), pfile)!= NULL) {
            
            if (strncmp(line_temp, field,no_of_bytes)==0)
            {
                strcpy(line, line_temp);
            }
        }
    }
    fclose(pfile);
    return line;
}

char * cmpr_airline(char *fieeld,int no_bytes)
{
    static const char filename[] = "UIDSTORE.txt";
    char one_line[20];
    char temp[20];
    char *linee =  (char *)malloc(sizeof(char) * 20);
    FILE *pfilee = fopen(filename,"r");
    char *pl = one_line;
    if (pfilee != NULL)
    {
        while (fgets(one_line, sizeof(one_line), pfilee)!= NULL) {
            bzero(temp, 20);
            strcpy(temp, one_line);
            memmove (pl, pl+3, strlen (pl+3) + 1);
            if (strncmp(pl, fieeld,no_bytes)==0)
            {
                strcpy(linee, temp);
            }
        }
    }
    fclose(pfilee);
    return linee;
}


