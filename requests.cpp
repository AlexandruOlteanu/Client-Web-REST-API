#include <bits/stdc++.h>
#include "helpers.h"
#include "requests.h"

using namespace std;

string compute_get_request(string host, string url, string query_params, string access_token, string cookie)
{
    string message;

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

    if (cookie != "") {
        message += "Cookie: " + cookie + "\r\n";
    }

    message += "\r\n";

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

    message += "Host: " + host + "\r\n";

    message += "Content-Type: " + content_type + "\r\n";

    message += "Content-Length: " + nr_to_string(data.size()) + "\r\n";

    if (access_token != "") {
       message += "Authorization: Bearer " + access_token + "\r\n";
    }
    
    if (cookie != "") {
        message += "Cookie: " + cookie + "\r\n";
    }

    message += "\r\n";

    message += data + "\r\n";

    return message;
}

string compute_delete_request(string host, string url, string content_type, string data, string access_token, string cookie)
{
    string message;

    message = "DELETE " + url + " HTTP/1.1" + "\r\n";

    message += "Host: " + host + "\r\n";

    if (access_token != "") {
       message += "Authorization: Bearer " + access_token + "\r\n";
    }
    
    if (cookie != "") {
        message += "Cookie: " + cookie + "\r\n";
    }

    message += "\r\n";

    message += data + "\r\n";

    return message;
}