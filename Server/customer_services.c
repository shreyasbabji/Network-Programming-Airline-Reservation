//
//  Customer_Services.c
//  tcpserver_airline_project
//
//  Created by Ifunanya Nnoka on 11/11/16.
//  Copyright © 2016 Ifunanya Nnoka. All rights reserved.
//

#include <stdio.h>
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

extern int errno;
int errexit(const char *format, ...);
char * extract_line(char *field,const char filename[],int no_of_bytes);
char * cmpr_object(char *fieeld,const char filename[],int no_bytes);
void create_customer_acc(void);
void create_booking_table(void);
int login_user(char* user,char* password);
int search_flight (const char *source, const char *dest);
int add_view(char* buffer);
void add_column(void);
int confirm_order_(char* t_id,char* buffer);
int seat_crosscheck(char *flight,int callback(void *data, int argc, char **argv, char **azColName));
void retrieve(char *buffer);
int retrieve_transaction(char *buffer);
static int seat_sort(void *data, int argc, char **argv, char **azColName);
static int seat_dup_check(void *filename, int argc, char **argv, char **azColName);
sqlite3 *db3;
int rc3;
int ok,k;
char buffa[1001];
char buf[256];
char call_bus[2701];
char *cb2 = call_bus;
int order_ok = 0;
int f_count = 0;
int no_of_flights=0;
char *pp = buf;

void Customer_funct(void* sock)
{
    char fromclient[2701];
    char toclient[2701];
    char depart[50];
    char arrive[50];
    char flight_[50];
    char airline[50];
    char total_seats[50];
    char price[50];
    char date_depart[50];
    char date_arrive[50];
    int t_seats, o_seats;
    char open_seats[50];
    char seat_number[50];
    char order_no[50];
    char cust_option;
    int transaction_num;
    char username[50];
    char user_temp[50];
    char password[50];
    char first_name[50];
    char last_name[50];
    char *Welcome_screen_cust = "Welcome to Airline reservation."\
    "Are you registered with us?\n 1. New User 2. Existing User ";
    FILE *pointer;
    int n1,num;
    int j = 0;
    int p = 0;
    int pass = 0;
    int true = 1;
    int ssock = (int) sock; /* slave server socket */
    /*Open Database*/
    rc3 = sqlite3_open("airline.db", &db3);
    if( rc3 ){
        fprintf(stderr, "Database failed to open: %s\n", sqlite3_errmsg(db3));
    }
    else
        fprintf(stderr, "Database opened successfully\n");
    //add_column();
    // create_customer_acc();
   // create_booking_table();
    while (true) {
    top:
        bzero(toclient,sizeof(toclient));
        printf("writing to the client\n");
        strcpy(toclient,Welcome_screen_cust);
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
        cust_option = fromclient[0];
        if (cust_option == '1') {
            //new user
    signup:  /*Request for username*/
            printf("writing to the client\n");
            bzero(toclient,2701);
            strcpy(toclient,"Enter new username: ");
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
            strcpy(username,fromclient);
            /*Request for password*/
            printf("writing to the client\n");
            bzero(toclient,2701);
            strcpy(toclient,"Enter new password: ");
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
            strcpy(password,fromclient);
            /*Request for First name*/
            printf("writing to the client\n");
            bzero(toclient,2701);
            strcpy(toclient,"Enter First name: ");
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
            strcpy(first_name,fromclient);
            /*Request for Last name*/
            printf("writing to the client\n");
            bzero(toclient,2701);
            strcpy(toclient,"Enter Last name: ");
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
            strcpy(last_name,fromclient);
            bzero(buffa, 1001);
            sprintf(buffa, "insert into customer_account values ('%s', '%s','%s','%s');",username,password,first_name,last_name);
            p = add_view(buffa);
            if (p == 1) {
                printf("writing to the client\n");
                bzero(toclient,2701);
                sprintf(toclient,"%s you have been successfully registered.\n"\
                        "Press Enter to login.",username);
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
            }
            else
            {
                printf("writing to the client\n");
                bzero(toclient,2701);
                sprintf(toclient,"Username %s not available."\
                        "Please choose a different username.",username);
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                goto signup;
            }
            goto login;
        }
        else if (cust_option == '2'){
            //existing user
    login: /*Request for username*/
            printf("writing to the client\n");
            bzero(toclient,2701);
            strcpy(toclient,"Enter username: ");
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
            strcpy(username,fromclient);
            /*Request for password*/
            printf("writing to the client\n");
            bzero(toclient,2701);
            strcpy(toclient,"Enter password: ");
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
            strcpy(password,fromclient);
            j = login_user(username,password);
            if (j == 1) {
                pass = 1;
                printf("writing to the client\n");
                bzero(toclient,2701);
                sprintf(toclient,"Welcome %s! What would you like to do?\n"\
                        "<<:: 1. Search Flights ::>>  <<:: 2. View bookings ::>> ",username);
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
            }else{
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient,"Incorrect password!");
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                    errexit("ERROR writing to socket\n");
                }
                goto login;
            }
        }else goto top;
        if (pass == 1) {
            pass = 0;
            printf("reading from the client\n");
            bzero(fromclient,sizeof(fromclient));
            n1 = read(ssock,fromclient,sizeof(fromclient));
            if (n1 < 0) {
                errexit("ERROR reading from socket %s\n", strerror(errno));
            }
            if (fromclient[0] == '1') {
                //search flights
        search: /*Request for source*/
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient,"Enter Departure Location: ");
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
                strcpy(depart,fromclient);
                /*Request for destination*/
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient,"Enter Destination: ");
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
                strcpy(arrive,fromclient);
                /*search flight*/
                p=0;
                f_count=0;
                /*Function searches for flight in database with those parameters and extracts flight number(s)*/
                //bzero(call_bus, sizeof(call_bus));
                p = search_flight(depart,arrive);
                int _count = (strlen(buf)/6.0);
                if (p != 1) {
                    printf("writing to the client\n");
                    bzero(toclient,2701);
                    strcpy(toclient,"No flights available through that route.\n"\
                           "Search more flights?\n Enter <<:: 1. Search Flights ::>>  <<:: 2. Exit ::>>");
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
                        goto search;
                    }
                }
                else{
                    //send search results to client
                    printf("writing to the client\n");
                    bzero(toclient,2701);
                    //puts results in client buffer
                    sprintf(toclient,"%s", call_bus);
                    //store extracted flight number(s) for later use
                   // strcpy(flight_, buf);
                    int i;
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }
                    printf("writing to the client\n");
                    bzero(toclient,2701);
                    char *point = toclient;
                    for (i=0; i<_count; i++) {
                        point += sprintf(point,"Enter %d. :::>> Book Flight %d\n",i+1,i+1);
                    }
                    point += sprintf(point,"Enter %d. :::>> Search More Flights",i+1);
                    //strcpy(toclient,"Enter <<:: 1. Book Flight ::>>  <<:: 2. Search More Flights ::>>");
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
                    if (atoi(&fromclient[0]) <=_count)
                    {
                        pp = &buf[0];
                        printf("fromclient: %d",atoi(&fromclient[0]));
                        for (i=0; i<_count; i++) {
                            if (atoi(&fromclient[0])== i+1){
                                strncpy(flight_,pp,6);
                                printf("pp: %s",pp);
                            }
                            printf("flight: %s",flight_);
                            memmove(pp, pp+6, strlen(pp+6) + 1);}
                        //book flight
                        bzero(buffa, 1001);
                        sprintf(buffa, "SELECT total_seats from DATA where name = '%s';",flight_);
                       // bzero(call_bus, sizeof(call_bus));
                        retrieve(buffa);
                        strcpy(total_seats, call_bus);
                        t_seats = atoi(total_seats);
                        bzero(buffa, 1001);
                        sprintf(buffa, "SELECT open_seats from DATA where name = '%s';",flight_);
                       // bzero(call_bus, sizeof(call_bus));
                        retrieve(buffa);
                        strcpy(open_seats, call_bus);
                        o_seats = atoi(open_seats);
                        
                        if (o_seats == 0) {
                        printf("writing to the client\n");
                        bzero(toclient,2701);
                        sprintf(toclient,"Unfortunately, Seats on flight %s are fully booked."\
                        "Enter <<:: 1. Search Flights ::>>  <<:: 2. Exit ::>>",flight_);
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
                            if (fromclient[0]== '1') {
                                goto search;
                            }else{
                            bzero(toclient,2701);
                            strcpy(toclient,"Have a Wonderfull day!!!!");
                            n1=write(ssock,toclient,strlen(toclient));
                            if (n1 <= 0){
                                errexit("ERROR writing to socket\n");
                            }
                            }true = 0;break;
                        }
                        num = t_seats - o_seats;
                        //check to see which seat numbers have been taken on a specific flight
                       // bzero(call_bus, sizeof(call_bus));
                        int s = seat_crosscheck(flight_,seat_sort);
                        if ((s != 1) ||(o_seats == t_seats)){
                        //no seats are boooked on that flight
                        printf("writing to the client\n");
                        bzero(toclient,2701);
                        sprintf(toclient,"Seats booked: %d\nChoose a Seat Number to Book Between Seat"\
                        " %d - Seat %d. Exclude Booked Seats.",num,num+1,t_seats);
                        }else{
                pick:   printf("writing to the client\n");
                        bzero(toclient,2701);
                        sprintf(toclient,"Seats already booked:\n%s\n"\
                        "Choose a seat number to book btw seat 1 - seat %d."\
                        "Exclude booked seats.",call_bus,t_seats);
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
                        if ((atoi(fromclient) < (num+1))||(atoi(fromclient) > t_seats)) {
                            goto pick;
                        }
                        /* Compare client input with already reserved seats*/
                        //Copies Reserved seats from database to a file
                        //bzero(call_bus, sizeof(call_bus));
                        s = seat_crosscheck(flight_,seat_dup_check);
                        //compares client input with each line
                        //of the text file containing records of already reserved seat numbers
                        char *bufff = cmpr_object(fromclient,"reserved_seats.txt",2);
                        printf("buff: %s",bufff);
                        if (*bufff == '\0') {
                            strcpy(seat_number,fromclient);
                        }else goto pick;
                        /*Clear file*/
                        pointer = fopen("reserved_seats.txt", "w");
                        fprintf(pointer, "");
                        fclose(pointer);
                        /*confirm booking*/
                        printf("writing to the client\n");
                        bzero(toclient,2701);
                        strcpy(toclient,"Enter <<:: 1. Complete Transaction ::>>  <<:: 2. Cancel ::>> ");
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
                            //complete transaction
                            //get transaction number
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT order_no from booked_flights;");
                           // bzero(call_bus, sizeof(call_bus));
                            transaction_num = retrieve_transaction(buffa);
                            //retrieve airline name from database
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT airline from DATA where name = '%s';",flight_);
                           // bzero(call_bus, sizeof(call_bus));
                            retrieve(buffa);
                            strcpy(airline, call_bus);
                            //retrieve price from database
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT price from DATA where name = '%s';",flight_);
                            //bzero(call_bus, sizeof(call_bus));
                            retrieve(buffa);
                            strcpy(price, call_bus);
                            //retrieve date of departure from database
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT date_depature from DATA where name = '%s';",flight_);
                           // bzero(call_bus, sizeof(call_bus));
                            retrieve(buffa);
                            strcpy(date_depart, call_bus);
                            //retrieve date of arrival from airline database
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT date_arrival from DATA where name = '%s';",flight_);
                           // bzero(call_bus, sizeof(call_bus));
                            retrieve(buffa);
                            strcpy(date_arrive, call_bus);
                            //retrieve first name database
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT First_name from customer_account where username = '%s';",username);
                         //   bzero(call_bus, sizeof(call_bus));
                            retrieve(buffa);
                            strcpy(first_name, call_bus);
                            //retrieve Last name database
                            bzero(buffa, 1001);
                            sprintf(buffa, "SELECT Last_name from customer_account where username = '%s';",username);
                           // bzero(call_bus, sizeof(call_bus));
                            retrieve(buffa);
                            strcpy(last_name, call_bus);
                            //insert into table
                            bzero(buffa, 1001);
                            sprintf(buffa, "insert into booked_flights values (%d, '%s','%s','%s','%s','%s','%s','%s',%s,%s,'%s','%s');",transaction_num,flight_,username,first_name,last_name,airline,depart,arrive,seat_number,price,date_depart,date_arrive);
                            p = add_view(buffa);
                            if (p == 1) {
                                //subtract seat number from database
                                bzero(buffa, 1001);
                                sprintf(buffa, "UPDATE DATA set open_seats = %d where name = '%s'",o_seats-1, flight_);
                              //  bzero(call_bus, sizeof(call_bus));
                                add_view(buffa);
                                printf("writing to the client\n");
                                bzero(toclient,2701);
                                sprintf(toclient,"Congratulations! Your flight reservations have been made.\n"\
                                       "Your Order Number is %d.\n"\
                                       "Press 1 to view your flight reservations.",transaction_num);
                                //send user a confirmation of transaction
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
                                    //display booking details
                                    //asprintf(&bus, "SELECT * from booked_flights where order_no = %d;",transaction_num);
                                    bzero(buffa, 1001);
                                    sprintf(buffa, "SELECT order_no AS 'Transaction No.',Flight AS 'Flight No.', First_name AS 'First Name',"\
                                            "Last_name AS 'Last Name',Airline,Source AS 'From',Destination AS 'To',seat_number AS 'Seat No.',"\
                                            "Date_depart AS 'Date of Depature',Date_arrive AS 'Date of Arrival',price "\
                                            "from booked_flights where order_no = %d;",transaction_num);

                                  //  bzero(call_bus, sizeof(call_bus));
                                    j = add_view(buffa);
                                    /*Sending Transaction Details*/
                                    printf("writing to the client\n");
                                    bzero(toclient,2701);
                                    strcpy(toclient,call_bus);
                                    n1=write(ssock,toclient,strlen(toclient));
                                    if (n1 <= 0){
                                        errexit("ERROR writing to socket\n");
                                    }
                                }
                            }
                            else{
                                /*Unable to book*/
                                printf("writing to the client\n");
                                bzero(toclient,2701);
                                strcpy(toclient,"We're sorry! There was an error while booking your flight.\n"\
                                       "Enter 1 to search more flights or any character to exit");
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
                                    goto search;
                                }else{
                                    printf("writing to the client\n");
                                    bzero(toclient,2701);
                                    strcpy(toclient,"Have a Wonderfull day!!!!");
                                    n1=write(ssock,toclient,strlen(toclient));
                                    if (n1 <= 0){
                                        errexit("ERROR writing to socket\n");
                                    }break;
                                }true = 0;
                            }
                        }
                        else if (fromclient[0] == '2'){
                        //cancel
                        //ask if user wants to search or exit
                        //act on response
                            /*exit or go to search*/
                            printf("writing to the client\n");
                            bzero(toclient,2701);
                            strcpy(toclient,"Enter 1 to search more flights or any character to exit");
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
                                goto search;
                            }
                            else
                            {   printf("writing to the client\n");
                                bzero(toclient,2701);
                                strcpy(toclient,"Have a Wonderfull day!!!!");
                                n1=write(ssock,toclient,strlen(toclient));
                                if (n1 <= 0){
                                    errexit("ERROR writing to socket\n");
                                }
                                break;
                            }true = 0;
                        }
                    }else if(atoi(&fromclient[0]) ==(_count+1)) goto search;
                }
            }else if (fromclient[0] == '2'){
                //view bookings
            view://ask for transaction number
                //use user info in customer_account to retrieve name of passenger
                //display booking
                /*Request for source*/
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient,"How would you like to retrieve your booking information?\n"\
                       " <<:: 1. View By Order No. ::>> <<:: 2. View All Bookings ::>>");
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
                //Use transaction number
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient,"Enter your order number: ");
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
                strcpy(order_no, fromclient);
                //check if transaction number exists and also if it matches user
                bzero(buffa, 1001);
                sprintf(buffa, "SELECT order_no from booked_flights;");
                j=0;
                //bzero(call_bus, sizeof(call_bus));
                j = confirm_order_(order_no,buffa);
                if (j != 1) {
                    /*Transaction number does not match our records*/
                    printf("writing to the client\n");
                    bzero(toclient,2701);
                    strcpy(toclient,"Order number not in ours records.\n Please Enter a valid order number.");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }goto view;
                }else{
                    transaction_num = atoi(order_no);
                    bzero(buffa, 1001);
                    sprintf(buffa, "SELECT user from booked_flights where order_no = %d;",transaction_num);
                   // bzero(call_bus, sizeof(call_bus));
                    retrieve(buffa);
                    strcpy(user_temp, call_bus);
                    if (strcmp(user_temp, username)!= 0) {
                        /*Transaction number assigned to a different client*/
                        printf("writing to the client\n");
                        bzero(toclient,2701);
                        strcpy(toclient,"Order number does not match your records");
                        n1=write(ssock,toclient,strlen(toclient));
                        if (n1 <= 0){
                            errexit("ERROR writing to socket\n");
                        }goto view;
                    }
                }
                bzero(buffa, 1001);
                sprintf(buffa, "SELECT order_no AS 'Transaction No.',Flight AS 'Flight No.', First_name AS 'First Name',"\
                "Last_name AS 'Last Name',Airline,Source AS 'From',Destination AS 'To',seat_number AS 'Seat No.',"\
                "Date_depart AS 'Date of Depature',Date_arrive AS 'Date of Arrival',price "\
                "from booked_flights where order_no = %s;",order_no);
                }else if (fromclient[0] == '2')
                {
                bzero(buffa, 1001);
                sprintf(buffa, "SELECT order_no AS 'Transaction No.',Flight AS 'Flight No.',"\
                "First_name AS 'First Name',Last_name AS 'Last Name',Airline,Source AS 'From',"\
                "Destination AS 'To',seat_number AS 'Seat No.',Date_depart AS 'Date of Depature'"\
                ",Date_arrive AS 'Date of Arrival',price from booked_flights where user = '%s';",username);
                }
               // bzero(call_bus, sizeof(call_bus));
                j = add_view(buffa);
                /*Sending Transaction Details*/
                printf("writing to the client\n");
                bzero(toclient,2701);
                sprintf(toclient,"\n%s",call_bus);
                //bzero(call_bus, sizeof(call_bus));
                n1=write(ssock,toclient,strlen(toclient));
                if (n1 <= 0){
                   errexit("ERROR writing to socket\n");
                }
                /*exit or go to search*/
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient," <<:: 1. Exit ::>>  <<:: 2. Search Flights ::>>");
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
                    bzero(toclient,2701);
                    strcpy(toclient,"Have a Wonderfull day!!!!");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }true = 0;
                }else if (fromclient[0] == '2') goto search;
            }
            else {
                /*exit or go back to correct entry*/
                printf("writing to the client\n");
                bzero(toclient,2701);
                strcpy(toclient,"Incorrect Entry!\n <<:: 1. Search Flights ::>> "\
                       "<<:: 2. View Bookings ::>> <<:: 3. Exit. ::>>");
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
                if (fromclient[0] == '1') goto search;
                else if (fromclient[0] == '2')goto view;
                else if (fromclient[0] == '3'){
                    bzero(toclient,2701);
                    strcpy(toclient,"Have a Wonderfull day!!!!");
                    n1=write(ssock,toclient,strlen(toclient));
                    if (n1 <= 0){
                        errexit("ERROR writing to socket\n");
                    }true = 0;
                }
            }
        }
    }
sqlite3_close(db3);

}

static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
    cb2 += sprintf(cb2,"%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if (argv[i] == NULL) {
            ok = 0;
        }else ok = 1;

    }
    cb2 += sprintf(cb2,"\n");
    return 0;
}

static int callback2(void *data, int argc, char **argv, char **azColName){
    int i;
    ok = 0;
    for(i=0; i<argc; i++){
        if(strcmp((const char*) data,argv[i])==0){
            ok = 1;
            order_ok = 1;
        }
    }
    return 0;
}

static int flight_identifier(void *data, int argc, char **argv, char **azColName){
    int i;
    f_count++;
    //counts how many flight matched the client's search parameters
    cb2 += sprintf(cb2,"\n<<<<++++FLIGHT %d.++++>>>>\n",f_count);
    for(i=0; i<argc; i++){
        //extracts flight parameters if there is a match
        cb2 += sprintf(cb2,"%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //used to extract flight number/name parameter
        if (strncmp("NAME", azColName[i], 4)==0) {
            //sprintf(buf,"%s\n", argv[i]);
           pp +=sprintf(pp,"%s", argv[i]);
    }
        //if null no flight matched client's search parameter
        if (argv[i] == NULL) {
            ok = 0;
        }else ok = 1;
    }
    cb2 += sprintf(cb2,"\n");
    return 0;

}

static int retr_gen_trans (void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        sprintf(call_bus,"%s",argv[i] ? argv[i] : "NULL");
        if (k <= atoi(argv[i]) ) {
            k++;
        }
    }
       return 0;
}
static int seat_dup_check(void *filename, int argc, char **argv, char **azColName)
{
    int i;
    FILE *fp;
    int stdout_bk; //fd for stdout backup
    stdout_bk = dup(fileno(stdout));
    fp = fopen(filename, "a");
    dup2(fileno(fp), fileno(stdout));
    for (i=0; i<argc; i++) {
        printf("%s",argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    dup2(stdout_bk, fileno(stdout)); //restore
    fclose(fp);
    return 0;
}
static int seat_sort(void *data, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        cb2 += sprintf(cb2," %s,",argv[i] ? argv[i] : "NULL");
        if (argv[i] == NULL) {
            ok = 0;
        }else ok = 1;
    }
    return 0;
}


void create_customer_acc(void)
{
    char *sql;
    char *zErrMsg = 0;
    /*Creates SQL statement*/
    sql = "Create table customer_account("\
    "username char primary key not null,"\
    "password         text not null,"\
    "First_name         text not null,"\
    "Last_name         text not null);";
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback, 0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
}
void create_booking_table(void)
{
    char *sql;
    char *zErrMsg = 0;
    
    /*Creates SQL statement*/
    sql = "Create table booked_flights("\
    "order_no  int primary key not null,"\
    "Flight       text not null,"\
    "user         text not null,"\
    "First_name   text not null,"\
    "Last_name    text not null,"\
    "Airline      text not null,"\
    "Source       text not null,"\
    "Destination  text not null,"\
    "seat_number  int  not null,"\
    "Price        real not null,"\
    "Date_depart  text not null,"\
    "Date_arrive  text not null);";
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback, 0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
}

void add_column(void)
{
    char *sql;
    char *zErrMsg = 0;
    sql ="ALTER TABLE booked_flights ADD COLUMN Date_depart char;";
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback, 0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    sql ="ALTER TABLE booked_flights ADD COLUMN Date_arrive char;";
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback, 0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    
}
int add_view(char* buffer)
{
    ok = 0;
    char *zErrMsg = 0;
    cb2 = &call_bus[0];
    char *sql;
    /*Creates SQL statement. Inserts information into SQL table named "data"*/
    sql = buffer;
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback, 0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        ok =1;
      //  fprintf(stdout, "New user added!\n");
    }
    return ok;
}

int login_user(char* user,char* password)
{
    char *zErrMsg = 0;
    char *sql;
    char buff[256];
    bzero(buff, 256);
    /*Creates SQL statement. Inserts information into SQL table named "data"*/
    sprintf(buff, "SELECT password from customer_account where username = '%s';",user);
    sql = buff;
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback2, (void*) password, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        fprintf(stdout, "password %s info retrieved successfully\n",user);
    }
    return ok;
}


int search_flight (const char *source, const char *dest)
{
    ok = 0;
    //f_count=0;
    cb2 = &call_bus[0];
    pp = &buf[0];
    char *zErrMsg = 0;
    char buff[256];
    char *sql;
    bzero(buff, 256);
    const char filename[] = "search_flight.txt";
    sprintf(buff, "SELECT name,airline,source,destination,price,open_seats AS'OPEN SEATS',"\
            "DATE_DEPATURE AS 'Date of Departure',DATE_ARRIVAL AS 'Date of Arrival'"\
            "from data where source = '%s' AND destination = '%s';",source,dest);
    sql = buff;
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, flight_identifier, (void*)filename, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        ok = 0;
    }
   // else
   //     ok =1;
    return ok;
}

int seat_crosscheck(char *flight,int sort_dup_check(void *data, int argc, char **argv, char **azColName)){
    char *zErrMsg = 0;
    cb2 = &call_bus[0];
    char buff[256];
    bzero(buff, 256);
    char *sql;

    //check to see which seat numbers have been taken on a specific flight
    sprintf(buff, "SELECT seat_number from booked_flights where flight = '%s';",flight);
    sql = buff;
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql,sort_dup_check, "reserved_seats.txt", &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return ok;
}
int confirm_order_(char* t_id,char *buffer)
{
    char *zErrMsg = 0;
    char *sql;
  //  cb2 = &call_bus[0];
    sql = buffer;
    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, callback2,(void*)t_id, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return order_ok;
}
   
void retrieve(char *buffer)
{
    char *zErrMsg = 0;
    char *sql;
    cb2 = &call_bus[0];
    /*Execute SQL statement*/
    sql = buffer;
    rc3 = sqlite3_exec(db3, sql, retr_gen_trans,0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}
int retrieve_transaction(char *buffer)
{
    k=1;
    char *zErrMsg = 0;
    char *sql;
    sql = buffer;
    cb2 = &call_bus[0];

    /*Execute SQL statement*/
    rc3 = sqlite3_exec(db3, sql, retr_gen_trans,0, &zErrMsg);
    if (rc3 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return k;
}
char * cmpr_object(char *fieeld,const char filename[],int no_bytes)
{
    //static const char filename[] = "UIDSTORE.txt";
    char one_line[20];
    char temp[20];
    char *linee =  (char *)malloc(sizeof(char) * 20);
    FILE *pfilee = fopen(filename,"r");
    // char *pl = one_line;
    if (pfilee != NULL)
    {
        while (fgets(one_line, sizeof(one_line), pfilee)!= NULL) {
            bzero(temp, 20);
            strcpy(temp, one_line);
            if (strncmp(one_line, fieeld,no_bytes)==0)
            {
                strcpy(linee, temp);
            }
        }
    }
    fclose(pfilee);
    return linee;
}
