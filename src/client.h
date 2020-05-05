#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define IP "3.8.116.10"
#define PORT 8080
#define DATA_TYPE "application/json"
#define HOST "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define REGISTER_PATH "/api/v1/tema/auth/register"
#define LOGIN_PATH "/api/v1/tema/auth/login"
#define LIBRARY_ACCESS "/api/v1/tema/library/access"
#define LIBRARY_BOOKS "/api/v1/tema/library/books"
#define LOGOUT_PATH "/api/v1/tema/auth/logout"
#define REGISTER 35
#define LOGIN 51


// registers the user to the server
// creates a json that is to be inserted into de get request that is also 
// done here and send depending on the code the function reads or just 
// logs in the user
void register_user(char* username, char* password, int sockfd, int code);

// parses the response from the server
// used after register/login
// returns a cookie or NULL
char* parse_resonse(int sockfd);

// the function that constructs the message for get_book
// it also sends the message to the server
// if the id is not a positive number exits with -1
void enter_library(char* cookie, int sockfd);

// parses the response from the server
// used after entering the library 
// returns a JWT or NULL
char* parse_resonse_enter_library(int sockfd);

// the function that constructs the message for get_books
// it also sends the message to the server
void get_books(char* token, int sockfd);

// the function that constructs the message for add_book
// it also sends the message to the server
// if the id is not a positive number exits with -1
int add_book(char* token, int sockfd);

// the function that constructs the message for logout
// it also sends the message to the server
void logout(char* token, int sockfd);

// the function that constructs the message for get_book
// it also sends the message to the server
// if the id is not a positive number exits with -1
int get_book(char* token, int sockfd);

// the function that constructs the message for delete_book
// it also sends the message to the server
// if the id is not a positive number exits with -1
int delete_book(char* token, int sockfd);

// gets the response from the server and displays it
void parse_resonse_generic(int sockfd);

// checks whether the string number is a positive (0 included) number
// and returns it in acc. If it's not anumber exits the function with -1
int check_for_int(char* number, int* acc);