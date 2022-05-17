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

string compute_get_request(string host, string url, string query_params, string access_token, string cookie)
{
    string message;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != "") {
        message += "GET " + url + "?" + query_params + " HTTP/1.1";
    } else {
        message += "GET " + url + " HTTP/1.1";
    }

    message += "\r\n";

    message += "Host: " + host + "\r\n";

    if (access_token != "") {
       message += "Authorization: Bearer " + access_token + "\r\n";
    }
    
    // // Step 4 (optional): add cookies
    if (cookie != "") {
        message += "Cookie: " + cookie + "\r\n";
    }
    // // Step 5: add new line at end of header
    message += "\r\n";
    // Step 6: add the actual payload data

    return message;
}

string nr_to_string(int nr) {
    string result = "";
    while (nr) {
        result = char(nr % 10 + '0') + result;
        nr /= 10; 
    }
    return result;
}

string compute_post_request(string host, string url, string content_type, string data, string access_token, string cookie)
{

    string message;

    message = "POST " + url + " HTTP/1.1" + "\r\n";
    // Step 1: write the method name, URL and protocol type
    message += "Host: " + host + "\r\n";

    message += "Content-Type: " + content_type + "\r\n";

    message += "Content-Length: " + nr_to_string(data.size()) + "\r\n";

    if (access_token != "") {
       message += "Authorization: Bearer " + access_token + "\r\n";
    }
    
    // // Step 4 (optional): add cookies
    if (cookie != "") {
        message += "Cookie: " + cookie + "\r\n";
    }
    // // Step 5: add new line at end of header
    message += "\r\n";
    // Step 6: add the actual payload data
    message += data + "\r\n";

    return message;
}

string compute_delete_request(string host, string url, string content_type, string data, string access_token, string cookie)
{
    string message;

    message = "DELETE " + url + " HTTP/1.1" + "\r\n";
    // Step 1: write the method name, URL and protocol type
    message += "Host: " + host + "\r\n";

    if (access_token != "") {
       message += "Authorization: Bearer " + access_token + "\r\n";
    }
    
    // // Step 4 (optional): add cookies
    if (cookie != "") {
        message += "Cookie: " + cookie + "\r\n";
    }
    // // Step 5: add new line at end of header
    message += "\r\n";
    // Step 6: add the actual payload data
    message += data + "\r\n";

    return message;
}