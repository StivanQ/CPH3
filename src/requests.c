#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message,line);
    if (cookies != NULL) {
        for(int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);
        }
    }
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message,line);
    if (cookies != NULL) {
        for(int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);
        }
    }
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    char *header = calloc(LINELEN, sizeof(char));
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(header, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(header,line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(header,line);
    int len = 0;
    int i;
    for(i = 0; i < body_data_fields_count; i++) {
        len += strlen(body_data[i]);
    }

    for(i = 0; i < body_data_fields_count; i++) {
        compute_message(body_data_buffer, body_data[i]);
    }

    sprintf(line, "Content-Length: %d", len);
    compute_message(header,line);

    if (cookies != NULL) {
        for(int i = 0; i < cookies_count; i++) {
            compute_message(header, cookies[i]);
        }
    }
    compute_message(message, header);
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);
    compute_message(message, "");
    free(line);
    free(header);
    free(body_data_buffer);
    return message;
}
