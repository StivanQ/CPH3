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

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
        
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

    // Ex 1.1: GET dummy from main server

    // char *compute_get_request(char *host, char *url, char *query_params,
    //                         char **cookies, int cookies_count)
    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                                    "/api/v1/dummy", NULL, NULL, 0);
        
   // printf("{%s}\n", message);

    // void send_to_server(int sockfd, char *message)
    send_to_server(sockfd, message);

    // char *receive_from_server(int sockfd)
    response = receive_from_server(sockfd);

    printf("{{{{%s}}}}\n", response);

    // Ex 1.2: POST dummy and print response from main server

    sockfd = open_connection("188.166.16.132", 80, AF_INET, SOCK_STREAM, 0);

    char* body_data[2];
    body_data[0] = "w/e mesaj";

    message = compute_post_request("api.openweathermap.org",
                                     "/api/v1/dummy", 
                                    "application/x-www-form-urlencoded", body_data, 
                                    1, NULL, 0);
    printf("{%s}\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("{{{{%s}}}}\n", response);

    // Ex 2: Login into main server
    // char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
    //                         int body_data_fields_count, char **cookies, int cookies_count)
    body_data[0] = "username=student&password=student";
    message = compute_post_request("api.openweathermap.org",
                                     "/api/v1/auth/login", 
                                    "application/x-www-form-urlencoded", body_data, 
                                    1, NULL, 0);


    printf("{%s}\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("{{{{%s}}}}\n", response);


    // Ex 3: GET weather key from main server


    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
