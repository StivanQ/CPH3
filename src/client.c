#include "client.h"

#define IP "3.8.116.10"
#define PORT 8080
#define DATA_TYPE "application/json"
#define HOST "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define REGISTER_PATH "/api/v1/tema/auth/register"
#define LOGIN_PATH "/api/v1/tema/auth/login"
#define REGISTER 35
#define LOGIN 51

int main(int argc, char *argv[])
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *response;
    char username[100];
    char password[100];
    char login_cookie[LINELEN];
    int sockfd;


    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    char* read_buffer;

    while(1) {
        read_buffer = calloc(LINELEN, sizeof(char));
        scanf("%s", read_buffer);

        if(strcmp(read_buffer, "exit") == 0) {
            close(sockfd);
            // free things
            break;
        } else if(strcmp(read_buffer, "register") == 0) {
            // register boss
            printf("username=");
            scanf("%s", username);
            // printf("%s\n", username);
            printf("password=");
            scanf("%s", password);
            // printf("%s\n", password);

            register_user(username, password, sockfd, REGISTER);

        } else if (strcmp(read_buffer, "login") == 0) {
            // login boss
            register_user(username, password, sockfd, LOGIN);
        } else if (strcmp(read_buffer, "enter_library") == 0) {
            // login boss
        } else if (strcmp(read_buffer, "get_books") == 0) {
            // login boss
        } else if (strcmp(read_buffer, "gen_book") == 0) {
            // login boss
        } else if (strcmp(read_buffer, "add_book") == 0) {
            // login boss
        } else if (strcmp(read_buffer, "delete_book") == 0) {
            // login boss
        } else if (strcmp(read_buffer, "logout") == 0) {
            // login boss
        } else {
            // comanda nu e buna
        }


        response = receive_from_server(sockfd);
        printf("[%s]\n", response);

        free(read_buffer);
    }

    return 0;
}


void register_user(char* username, char* password, int sockfd, int code) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    char* message;

    message = compute_post_request(HOST,
                                    code == REGISTER ? 
                                    REGISTER_PATH : LOGIN_PATH, 
                                    DATA_TYPE,
                                    &serialized_string, 
                                    1, 
                                    NULL, 
                                    0);

    send_to_server(sockfd, message);

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void parse_resonse(int sockfd) {
    char* response;
    response = receive_from_server(sockfd);
}