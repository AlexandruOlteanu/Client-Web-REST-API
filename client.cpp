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

using namespace std;

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

    char host_ip[] = "34.241.4.235";

    string message;
    string command;
    string response;
    int32_t server_socket = -1;
    server_socket = open_connection(host_ip, 8080, PF_INET, SOCK_STREAM, 0);
    ERROR(server_socket < 0, "Error, oppening connection failed");

    add_default_command_options();
    int32_t count = 0;

    while (true) {
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
            break;       
        }

        if (command == "reqister") {

            

        }

    }

    shutdown(server_socket, SHUT_RDWR);

    return 0;
}