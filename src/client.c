#include "client.h"

int connected = 0;
char last_message[BUFLEN];
int last_message_type;
int last_message_waiting = 0;

int main(int argc, char *argv[])
{
    char *response;
    char username[100];
    char password[100];
    char cookie[LINELEN];
    char *token = calloc(LINELEN, sizeof(char));
    int sockfd;
    int err;
    char* read_buffer;

    while(1) {
        read_buffer = calloc(LINELEN, sizeof(char));

        
        if(connected == 0) {
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
            connected = 1;

            if(last_message_waiting == 1) {
	            send_to_server(sockfd, last_message);

	            if(last_message_type == LOGIN) {
	            	response = parse_resonse(sockfd);
		            if(response != NULL) {
		                strcpy(cookie, response);
		            }
	            } else if (last_message_type == ENTER_LIBRARY) {
					response = parse_resonse_enter_library(sockfd);
            		if(response != NULL) {
		                strcpy(token, response);
		                free(response);
		            }
	            } else {
					parse_resonse_generic(sockfd);
	            }
	            last_message_waiting = 0;
        	}
            free(read_buffer);
            continue;
        }


        scanf("%s", read_buffer);

        if(strcmp(read_buffer, "exit") == 0) {
            close(sockfd);
            free(read_buffer);
            break;
        } else if(strcmp(read_buffer, "register") == 0) {
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            register_user(username, password, sockfd, REGISTER);
            response = parse_resonse(sockfd);

        } else if (strcmp(read_buffer, "login") == 0) {
            register_user(username, password, sockfd, LOGIN);
            response = parse_resonse(sockfd);
            if(response != NULL) {
                strcpy(cookie, response);
            }
        } else if (strcmp(read_buffer, "enter_library") == 0) {
            enter_library(cookie, sockfd);
            response = parse_resonse_enter_library(sockfd);
            if(response != NULL) {
                strcpy(token, response);
                free(response);
            }
        } else if (strcmp(read_buffer, "get_books") == 0) {
            get_books(token, sockfd);
            parse_resonse_generic(sockfd);
        } else if (strcmp(read_buffer, "get_book") == 0) {
            err = get_book(token, sockfd);
            if(err != -1)
                parse_resonse_generic(sockfd);
        } else if (strcmp(read_buffer, "add_book") == 0) {
            err = add_book(token, sockfd);
            if(err != -1)
                parse_resonse_generic(sockfd);
        } else if (strcmp(read_buffer, "delete_book") == 0) {
            err = delete_book(token, sockfd);
            if(err != -1)
                parse_resonse_generic(sockfd);
        } else if (strcmp(read_buffer, "logout") == 0) {
            logout(cookie, sockfd);
            memset(token, 0, strlen(token));
            parse_resonse_generic(sockfd);
        } else {
            printf("Nu se recunoaste comanda introdusa.\n");
            free(read_buffer);
            continue;
        }

        free(read_buffer);
    }
    free(token);
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
    strcpy(last_message, message);
    last_message_type = REGISTER;
    last_message_waiting = 1;
    send_to_server(sockfd, message);
    free(message);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void enter_library(char* cookie, int sockfd) {
    char* message;

    char* cookieBun = calloc(200, sizeof(char));
    sprintf(cookieBun, "Cookie: connect.sid=%s", cookie);
    message = compute_get_request(HOST,
                                    LIBRARY_ACCESS,
                                    NULL,
                                    &(cookieBun),
                                    1);
    strcpy(last_message, message);
    last_message_type = ENTER_LIBRARY;
    last_message_waiting = 1;
    send_to_server(sockfd, message);
    free(cookieBun);
    free(message);
}

void get_books(char* token, int sockfd) {
    char* message;

    char* tokenHeader = calloc(500, sizeof(char));
    sprintf(tokenHeader, "Authorization: Bearer %s", token);

    message = compute_get_request(HOST,
                                    LIBRARY_BOOKS,
                                    NULL,
                                    &(tokenHeader),
                                    1);
    strcpy(last_message, message);
    last_message_type = GENERIC;
    last_message_waiting = 1;
    send_to_server(sockfd, message);
    free(tokenHeader);
    free(message);
}

char* parse_resonse(int sockfd) {
    char* response;
    response = receive_from_server(sockfd);
    char aux[BUFLEN];
    strcpy(aux, response);
    free(response);
    char* ptr = strtok(aux, "\r\n");
    char* cookie_ptr = NULL;
    char* cookie = NULL;
    while(ptr != NULL) {
        printf("%s\n", ptr);
        if((cookie_ptr = strstr(ptr, "connect.sid")) != NULL) {
            cookie_ptr += 12;
            break;
        }
        ptr = strtok(NULL, "\n");
    }
    if(cookie_ptr != NULL) {
        cookie = strtok(cookie_ptr, " ");
        cookie[strlen(cookie) - 1] = '\0';
    }
    return cookie;
}

int add_book(char* token, int sockfd) {
    char page_count[100];

    char *title = NULL;
    char *author = NULL;
    char *genre = NULL;
    char *publisher = NULL;

    size_t len = 0;

    getchar();
    printf("title=");
    getline(&title, &len, stdin);
    len = 0;

    printf("author=");
    getline(&author, &len, stdin);
    len = 0;

    printf("genre=");
    getline(&genre, &len, stdin);
    len = 0;

    printf("publisher=");
    getline(&publisher, &len, stdin);
    len = 0;

    printf("page_count=");
    scanf("%s", page_count);

    // getline citeste si '\n' si il pune in string si nu vrem sa il avem
    title[strlen(title) - 1] = '\0';
    author[strlen(author) - 1] = '\0';
    publisher[strlen(publisher) - 1] = '\0';
    genre[strlen(genre) - 1] = '\0';

    int page_count_integer = 0;
    if(check_for_int(page_count, &page_count_integer) == -1) {
        printf("Nu ai introdus un numar pozitiv.\nIntrodu comanda din nou\n");
        return -1;
    }

    char* tokenHeader = calloc(500, sizeof(char));
    sprintf(tokenHeader, "Authorization: Bearer %s", token);

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count_integer);
    serialized_string = json_serialize_to_string_pretty(root_value);

    char* message;

    message = compute_post_request(HOST,
                                LIBRARY_BOOKS, 
                                DATA_TYPE,
                                &serialized_string, 
                                1, 
                                &tokenHeader, 
                                1);
    strcpy(last_message, message);
    last_message_type = GENERIC;
    last_message_waiting = 1;
    send_to_server(sockfd, message);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(message);
    free(tokenHeader);

    free(title);
    free(author);
    free(publisher);
    free(genre);
    // free(page_count);

    return 0;
}

char* parse_resonse_enter_library(int sockfd) {
    char* response = NULL;
    char aux[BUFLEN];
    response = receive_from_server(sockfd);

    strcpy(aux, response);

    printf("%s\n", aux);
    if(strstr(aux, "401 Unauthorized") != NULL) {
        return NULL;
    }

    char* ptr = strtok(aux, "\r\n");
    char* ptr_prim = NULL;
    while(ptr != NULL) {
        ptr_prim = ptr;
        ptr = strtok(NULL, "\n");
    }

    char* intermediar = calloc(500, sizeof(char));

    if(ptr_prim != NULL) {
        JSON_Value* token_json = json_parse_string(ptr_prim);
        JSON_Object* root_object = json_value_get_object(token_json);
        response = json_object_get_string(root_object, "token");
        strcpy(intermediar, response);
        json_value_free(token_json);
    }

    return intermediar;
}

void logout(char* cookie, int sockfd) {
    char* message;
    char* cookieBun = calloc(200, sizeof(char));
    sprintf(cookieBun, "Cookie: connect.sid=%s", cookie);
    // char *compute_get_request(char *host, char *url, char *query_params,
    //                         char **cookies, int cookies_count)
    message = compute_get_request(HOST,
                                    LOGOUT_PATH,
                                    NULL,
                                    &(cookieBun),
                                    1);
    strcpy(last_message, message);
    last_message_type = GENERIC;
    last_message_waiting = 1;
    send_to_server(sockfd, message);
    free(message);
    free(cookieBun);
}

int get_book(char* token, int sockfd) {
    char* message;

    int id;
    char id_string[100];
    printf("id=");
    scanf("%s", id_string);
    if(check_for_int(id_string, &id) == -1) {
        printf("Nu ai introdus un numar pozitiv.\nIntrodu comanda din nou\n");
        return -1;
    }
    char* tokenHeader = calloc(500, sizeof(char));
    sprintf(tokenHeader, "Authorization: Bearer %s", token);
    char path[100];
    sprintf(path, "/api/v1/tema/library/books/%d", id);
    message = compute_get_request(HOST,
                                    path,
                                    NULL,
                                    &(tokenHeader),
                                    1);
    strcpy(last_message, message);
    last_message_type = GENERIC;
    last_message_waiting = 1;
    send_to_server(sockfd, message);
    free(message);
    free(tokenHeader);
    return 0;
}

int delete_book(char* token, int sockfd) {
    char* message;

    int id;
    char id_string[100];
    printf("id=");
    scanf("%s", id_string);
    if(check_for_int(id_string, &id) == -1) {
        printf("Nu ai introdus un numar pozitiv.\nIntrodu comanda din nou\n");
        return -1;
    }
    char* tokenHeader = calloc(500, sizeof(char));
    sprintf(tokenHeader, "Authorization: Bearer %s", token);
    char path[100];
    sprintf(path, "/api/v1/tema/library/books/%d", id);
    message = compute_delete_request(HOST,
                                    path,
                                    NULL,
                                    &(tokenHeader),
                                    1);

    strcpy(last_message, message);
    last_message_waiting = 1;
    last_message_type = GENERIC;
    send_to_server(sockfd, message);
    free(message);
    free(tokenHeader);
    return 0;
}

void parse_resonse_generic(int sockfd) {
    char* response;
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    free(response);
}

int check_for_int(char* number, int* acc) {
    int i;
    *acc = 0;
    for(i = 0; i < strlen(number); i++){
        if(number[i] < '0' || number[i] > '9') {
            return -1;
        }
        *acc = *acc * 10 + (number[i] - '0');;
    }
    return 0;
}