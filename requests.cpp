#include <bits/stdc++.h>
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

using namespace std;

char *compute_get_request(char *host, char *url, char *query_params, char **jwt_token, int token_count, 
                            char **cookies, int cookies_count)
{
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (jwt_token != NULL && token_count > 0) {
       sprintf(line, "Authorization: Bearer ");
       for (int i = 0; i < token_count; ++i) {
           sprintf(line + strlen(line), "%s ", jwt_token[i]);
       }
       compute_message(message, line);
    }
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0) {
       sprintf(line, "Cookie: ");
       for (int i = 0; i < cookies_count; ++i) {
           sprintf(line + strlen(line), "%s ", cookies[i]);
       }
       line[strlen(line) - 1] = '\n';
       compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data, 
                            int body_data_fields_count, char **jwt_token, int token_count, char **cookies, int cookies_count)
{
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char *) calloc(LINELEN, sizeof(char));

    for (int i = 0; i < body_data_fields_count; ++i) {
        strcat(body_data_buffer, body_data[i]);
        if (i < body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (jwt_token != NULL && token_count > 0) {
       sprintf(line, "Authorization: Bearer ");
       for (int i = 0; i < token_count; ++i) {
           sprintf(line + strlen(line), "%s ", jwt_token[i]);
       }
       compute_message(message, line);
    }

    sprintf(line, "Content-Length: %d", (int) strlen(body_data_buffer));
    compute_message(message, line);
    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies_count > 0) {
       sprintf(line, "Cookie: ");
       for (int i = 0; i < cookies_count; ++i) {
           sprintf(line + strlen(line), "%s ", cookies[i]);
       }
       line[strlen(line) - 1] = '\0';
       compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}