#include <bits/stdc++.h>
#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "error.h"
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

vector<string> command_options;

void add_default_command_options() {
    command_options.push_back("register");
    command_options.push_back("login");
    command_options.push_back("enter_library");
    command_options.push_back("get_books");
    command_options.push_back("get_book");
    command_options.push_back("add_book");
    command_options.push_back("delete_book");
    command_options.push_back("logout");
    command_options.push_back("exit");
}

void add_command_option(string option) {
    command_options.push_back(option);
}


int main(int argc, char *argv[])
{
    ERROR(argc != 1, "Error, number of arguments");

    char server_host_ip[] = "34.241.4.235";
    int32_t server_port = 8080; 

    string message;
    string command;
    string response;

    add_default_command_options();
    int32_t count = 0;

    string current_cookie = "";
    string current_token = "";

    while (true) {
        int32_t server_socket = -1;
        server_socket = open_connection(server_host_ip, server_port, PF_INET, SOCK_STREAM, 0);
        ERROR(server_socket < 0, "Error, oppening connection failed");

        cout << "Enter one of the commands : ";
        count = 0;
        for (auto option : command_options) {
            ++count;
            cout << count << ") " << option << " ";
        }
        cout << '\n';

        getline(cin, command);
        bool command_found = false;
        for (auto option : command_options) {
            if (command == option) {
                command_found = true;
                break;
            }
        }
        if (!command_found) {
            cout << "Wrong command, please try again\n";
            continue;
        }
        if (command == "exit") {
            shutdown(server_socket, SHUT_RDWR);
            break;       
        }

        if (command == "register") {

            json info;
            string username, password;
            cout << "username=";
            getline(cin, username);
            cout << "password=";
            getline(cin, password);
            info["username"] = username;
            info["password"] = password;

            char **data_matrix = (char **)malloc(sizeof(char *));
            data_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(data_matrix[0], info.dump().c_str());
            string server_request = compute_post_request(server_host_ip, "/api/v1/tema/auth/register", "application/json", data_matrix, 1, NULL, 0, NULL, 0);
            cout << server_request << '\n';
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            cout << result << '\n';
            if (result.find("error") != string::npos) {
                cout << "Username is already taken\n";
            }
            else {
                cout << "New user with username: " << username << " was created!\n";
            }
            continue;
        }
        
        if (command == "login") {
            json info;
            string username, password;
            cout << "username=";
            getline(cin, username);
            cout << "password=";
            getline(cin, password);
            info["username"] = username;
            info["password"] = password;

            char **data_matrix = (char **)malloc(sizeof(char *));
            data_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(data_matrix[0], info.dump().c_str());
            string server_request = compute_post_request(server_host_ip, "/api/v1/tema/auth/login", "application/json", data_matrix, 1, NULL, 0, NULL, 0);
            cout << server_request << '\n';
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            cout << result << '\n';
            if (result.find("error") != string::npos) {
                cout << "Invalid Login Data\n";
            }
            else {
                current_cookie = strstr(result.c_str(), "connect.sid=");
                for (int i = 0; i < current_cookie.size(); ++i) {
                    if (current_cookie[i] == ';') {
                        current_cookie = current_cookie.substr(0, i);
                        break;
                    }
                }
                cout << "Login succesfully!\n";
            }
            continue;
        }

        if (command == "enter_library") {

            char **cookies_matrix = (char **)malloc(sizeof(char *));
            cookies_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(cookies_matrix[0], current_cookie.c_str());
            string server_request = compute_get_request(server_host_ip, "/api/v1/tema/library/access", NULL, NULL, 0, cookies_matrix, 1);
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            result = strstr(result.c_str(), "{");
            json info;

            info = json::parse(result);
            current_token = info["token"];
            cout << current_token << '\n';
            if (result.find("error") != string::npos) {
                cout << "Invalid Access\n";
            }
            else {
                cout << "Access Success!\n";
            }
            continue;
        }

        if (command == "get_books") {
            char **cookies_matrix = (char **)malloc(sizeof(char *));
            cookies_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(cookies_matrix[0], current_cookie.c_str());

            char **jwt_matrix = (char **)malloc(sizeof(char *));
            jwt_matrix[0] = (char *)malloc(2000* sizeof(char));
            strcpy(jwt_matrix[0], current_token.c_str());

            string server_request = compute_get_request(server_host_ip, "/api/v1/tema/library/books", NULL, jwt_matrix, 1, cookies_matrix, 1);
            cout << server_request << '\n';
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            cout << result << '\n';

            if (result.find("error") != string::npos) {
                cout << "Failed to get books\n";
            }
            else {
                cout << "Success in getting the books!\n";
            }
            continue;
        }

        if (command == "get_book") {
            
            cout << "id=";
            string id_book;
            cin >> id_book;

            string url = "/api/v1/tema/library/books/" + id_book;

            char **cookies_matrix = (char **)malloc(sizeof(char *));
            cookies_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(cookies_matrix[0], current_cookie.c_str());

            char **jwt_matrix = (char **)malloc(sizeof(char *));
            jwt_matrix[0] = (char *)malloc(2000* sizeof(char));
            strcpy(jwt_matrix[0], current_token.c_str());


            string server_request = compute_get_request(server_host_ip, (char *) url.c_str(), NULL, jwt_matrix, 1, cookies_matrix, 1);
            cout << server_request << '\n';
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            cout << result << '\n';

            if (result.find("error") != string::npos) {
                cout << "Failed to get book with Id: " << id_book << '\n';
            }
            else {
                cout << "Success in getting the book with Id: " << id_book << '\n';
            }

            continue;
        }

        if (command == "add_book") {

            json info;

            string title;
            string author;
            string genre;
            string publisher;
            string page_count;

            cout << "title=";
            getline(cin, title);
            info["title"] = title;

            cout << "author=";
            getline(cin, author);
            info["author"] = author;


            cout << "genre=";
            getline(cin, genre);
            info["genre"] = genre;


            cout << "publisher=";
            getline(cin, publisher);
            info["publisher"] = publisher;


            cout << "page_count=";
            getline(cin, page_count);
            info["page_count"] = page_count;

            char **data_matrix = (char **)malloc(sizeof(char *));
            data_matrix[0] = (char *)malloc(2000 * sizeof(char));
            strcpy(data_matrix[0], info.dump().c_str());

            char **cookies_matrix = (char **)malloc(sizeof(char *));
            cookies_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(cookies_matrix[0], current_cookie.c_str());

            char **jwt_matrix = (char **)malloc(sizeof(char *));
            jwt_matrix[0] = (char *)malloc(2000* sizeof(char));
            strcpy(jwt_matrix[0], current_token.c_str());

            string server_request = compute_post_request(server_host_ip, "/api/v1/tema/library/books", "application/json", data_matrix, 1,  
                                    jwt_matrix, 1, cookies_matrix, 1);
            cout << server_request << '\n';
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            cout << result << '\n';

            if (result.find("error") != string::npos) {
                cout << "Failed to add book\n\n\n";
            }
            else {
                cout << "Success in adding books!\n";
            }
            continue;
        }

        if (command == "delete_book") {

            cout << "id=";
            string id_book;
            cin >> id_book;

            string url = "/api/v1/tema/library/books/" + id_book;

            char **cookies_matrix = (char **)malloc(sizeof(char *));
            cookies_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(cookies_matrix[0], current_cookie.c_str());

            char **jwt_matrix = (char **)malloc(sizeof(char *));
            jwt_matrix[0] = (char *)malloc(2000* sizeof(char));
            strcpy(jwt_matrix[0], current_token.c_str());

            string server_request = compute_delete_request(server_host_ip, (char *) url.c_str(), NULL, 
                                    NULL, 0, jwt_matrix, 1, cookies_matrix, 1);
            cout << server_request << '\n';
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);
            cout << result << '\n';

            if (result.find("error") != string::npos) {
                cout << "Failed to delete book with Id: " << id_book << '\n';
            }
            else {
                cout << "Success in deleting the book with Id: " << id_book << '\n';
            }

            continue;
        }

        if (command == "logout") {

            char **cookies_matrix = (char **)malloc(sizeof(char *));
            cookies_matrix[0] = (char *)malloc(100 * sizeof(char));
            strcpy(cookies_matrix[0], current_cookie.c_str());

            char **jwt_matrix = (char **)malloc(sizeof(char *));
            jwt_matrix[0] = (char *)malloc(2000* sizeof(char));
            strcpy(jwt_matrix[0], current_token.c_str());

            string server_request = compute_get_request(server_host_ip, "/api/v1/tema/auth/logout", NULL, jwt_matrix, 1, cookies_matrix, 1);
            cout << server_request << '\n';
            send_to_server(server_socket, (char *) server_request.c_str());
            string result = receive_from_server(server_socket);
            cout << result << '\n';
            if (result.find("error") != string::npos) {
                cout << "Invalid Logout\n";
            }
            else {
                cout << "Logout Success!\n";
            }
            continue;

        }

        

        

    }

    return 0;
}