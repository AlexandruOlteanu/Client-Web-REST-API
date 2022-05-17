#ifndef _REQUESTS_
#define _REQUESTS_
using namespace std;
// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
string compute_get_request(string host, string url, string query_params, string access_token, string cookie);

// computes and returns a POST request string (cookies can be NULL if not needed)
string compute_post_request(string host, string url, string content_type, string data, string access_token, string cookie);

string compute_delete_request(string host, string url, string content_type, string data, string access_token, string cookie);

#endif
