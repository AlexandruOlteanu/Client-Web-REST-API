#include <bits/stdc++.h>   
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "error.h"
#include "json.hpp"

using namespace std;

#define MAX_SIZE 1000

using json = nlohmann::json;

vector<string> command_options;

/**
 * Comenzile default pe care le are userul initial
 * */
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

/**
 * In viitor se pot adauga noi functionalitati, astfel 
 * este folositoare o functie care sa adauge o noua comanda cand
 * este nexesar
 * */
void add_command_option(string option) {
    command_options.push_back(option);
}

/**
 * Drepturi de autentificare necesare in unele comenzi sau in incercarea spre
 * exemplu de logare cand suntem deja logati
 * */
struct needed_rights {
    bool loged_in_on_server = false;
};

/**
 * Creeaza prompt-ul pentru introducerea datelor utilizatorului
 * */
json get_user_credentials() {
    json info;
    string username, password;
    cout << "username=";
    cin >> username;
    cout << "password=";
    cin >> password;
    info["username"] = username;
    info["password"] = password;
    return info;
}

/**
 * Din mesajul primit de la server se extrage in mod corect cookie-ul
 * primit
 * */
string extract_cookie(string initial) {
    string result = initial;
    result = strstr(initial.c_str(), "connect.sid=");

    int i = 0;
    for (auto c : result) {
        if (c == ';') {
            return result.substr(0, i);
        }
        ++i;
    }
    return "";
}

/**
 * Link-uri ajutoatoare pentru fiecare comanda in parte plus portul si host-ul 
 * ip initial
 * */
struct server_links {
    int32_t server_port = 8080;
    char host_ip[MAX_SIZE] = "34.241.4.235";
    string register_url = "/api/v1/tema/auth/register";
    string login_url = "/api/v1/tema/auth/login";
    string enter_library_url = "/api/v1/tema/library/access";
    string get_books_url = "/api/v1/tema/library/books";
    string get_book_url = "/api/v1/tema/library/books/";
    string add_book_url = "/api/v1/tema/library/books";
    string delete_book_url = "/api/v1/tema/library/books/";
    string logout_url = "/api/v1/tema/auth/logout";
    string json_c_type = "application/json";
};

// Daca se doreste afisarea mesajelor de la server, se poate pune pe true aceasta variabila
bool get_server_results = false;

int main(int argc, char *argv[])
{
    ERROR(argc != 1, "Error, number of arguments");

    string message;
    string command;
    string response;

    add_default_command_options();
    int32_t count = 0;

    string current_cookie = "";
    string current_token = "";

    needed_rights rights;

    server_links server;
    
    while (true) {
        // Deschidem conexiunea pe serverul corespunzator, cu parametrii necesari pentru o conexiune IP
        int32_t server_socket = -1;
        server_socket = open_connection(server.host_ip, server.server_port, PF_INET, SOCK_STREAM, 0);
        ERROR(server_socket < 0, "Error, oppening connection failed");

        //Afisam comenzile pe care le poate alege utilizatorul la fiecare pas
        cout << "\nEnter one of the commands : ";
        count = 0;
        for (auto option : command_options) {
            ++count;
            cout << count << ") " << option << " ";
        }
        cout << '\n';

        cin >> command;
        bool command_found = false;
        for (auto option : command_options) {
            if (command == option) {
                command_found = true;
                break;
            }
        }
        // Daca aceasta comanda este gresita, se repeta intrebarea de alegere
        if (!command_found) {
            cout << "Wrong command, please try again\n";
            continue;
        }
        // In cazul in care comanda este de exit, se face shutdown serverului si se incheie 
        // programul
        if (command == "exit") {
            int32_t check_ret = shutdown(server_socket, SHUT_RDWR);
            ERROR(check_ret < 0, "Failed to close server");
            break;       
        }

        // In cazul comenzii de register, se verifica daca suntem deja logati, daca da, 
        // se afiseaza eroarea corespunzatoare, altfel se preiau username-ul si parola 
        // introduse de utilizator si se creeaza request-ul de tip POST
        // la adresa serverului. In cazul unei coliziuni de username cu un cont deja existent, 
        // se afiseaza eroarea primind mesajul de la server. In caz contrar, un nou utilizator este
        // inregistrat 
        if (command == "register") {
            
            if (rights.loged_in_on_server) {
                cout << "Error, already logged in!\n";
                continue;
            }

            json info = get_user_credentials();

            string server_request = compute_post_request(server.host_ip, server.register_url, server.json_c_type, info.dump(), "", "");

            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                cout << "Username is already taken!\n";
            }
            else {
                cout << "New user with username: " << info["username"].dump() << " was created!\n";
            }
            continue;
        }
        
        // Analog procedam la comanda de login, cu diferenta ca acum vom primi eroare in cazul in care 
        // utilizatorul nu exista sau parola este gresita. De asemenea, la o logare corecta, vom primi
        // si un cookie de sesiune care va fi extras si stocat intr-un string. Nu avem nevoie de mai 
        // multe cookie-uri deoarece putem fi logati pe maxim un cont la un moment dat.
        if (command == "login") {
            
            if (rights.loged_in_on_server) {
                cout << "Error, already logged in!\n";
                continue;
            }

            json info = get_user_credentials();

            string server_request = compute_post_request(server.host_ip, server.login_url, server.json_c_type, info.dump(), "", "");
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                cout << "Error, Credentials are not good!\n";
            }
            else {
                current_cookie = extract_cookie(result);
                rights.loged_in_on_server = true;
                cout << "Login succesfully!\n";
            }
            continue;
        }

        // Intrarea in librarie si primirea accesului se realizeaza corect doar in cazul in care suntem 
        // logati. Astfel, trimitem un request de tip GET cu cookie-ul corespunzator care este validat de server
        // ca apartinand sesiunii corecte. In caz pozitiv, vom primi un token jwt de acces care va fi apoi stocat
        // la fel ca si cookie-ul. In caz contrar, se va afisa eroarea corespunzatoare.
        if (command == "enter_library") {
            
            string server_request = compute_get_request(server.host_ip, server.enter_library_url, "", "", current_cookie);
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                cout << "Invalid Acceess, Not Logged In!\n";
            }
            else {
                cout << "Succesfully entered the Library!\n";
                result = strstr(result.c_str(), "{");
                json info;

                info = json::parse(result);
                current_token = info["token"];
            }
            continue;
        }

        // Pentru a avea acces la comanda de afisare a tuturor cartilor trebuie sa
        // fim atat logati dar si sa avem acces la biblioteca. Astfel, atat cookie-ul
        // cat si token-ul jwt trebuie trimise catre server in acest request de tip GET.
        // Eroarea primita va fi fie cu privire la logare, fie la accesul nepermis.
        // La o comanda de succes, vom putea vedea id-ul si titlul fiecarei carti 
        // prezente in biblioteca
        if (command == "get_books") {

            string server_request = compute_get_request(server.host_ip, server.get_books_url, "", current_token, current_cookie);
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                cout << "Failed to get books, Not Authorized\n";
            }
            else {
                cout << result.substr(result.find("[")) << '\n';
                cout << "Success in getting the books!\n";
            }
            continue;
        }

        // In cazul comenzii de get_book, trebuie afisate informatiile particulare ale unei carti.
        // Astfel, trebuie sa avem ca si anterior, atat o sesiune de log in dar si un token jwt. 
        // In caz de succes, vom afisa informatiile despre cartea dorita daca id-ul acesteia 
        // exista, in caz contrar eroare.
        if (command == "get_book") {
            
            cout << "id=";
            string id_book;
            cin >> id_book;

            string url = server.get_book_url + id_book;

            string server_request = compute_get_request(server.host_ip, url, "", current_token, current_cookie);
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                if (result.find("Authorization") != string::npos) {
                    cout << "Failed to get book with Id: " << id_book << ", Not Authorized!\n";
                }
                else {
                    cout << "Book with Id: " << id_book << " doesn't exist!\n";
                }
            }
            else {
                cout << result.substr(result.find("[")) << '\n';
                cout << "Success in getting the book with Id: " << id_book << '\n';
            }

            continue;
        }

        // In cadrul comenzii de adaugare al unei carti, am format obiectul json corespunzator
        // tuturor datelor necesare apoi l-am trimis serverului ca string. Serverul a verificat 
        // calitatea datelor introduse, producand eroare in cazul in care numarul de pagini nu este 
        // un numar intreg. Am tratat toate aceste erori si in caz de succes, am afisat mesajul 
        // de validare.
        if (command == "add_book") {

            json info;

            string title;
            string author;
            string genre;
            string publisher;
            string page_count;

            getchar();
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

            string server_request = compute_post_request(server.host_ip, server.add_book_url, server.json_c_type, info.dump(), current_token, current_cookie);
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                if (result.find("Authorization") != string::npos) {
                    cout << "Failed to add book, Not Authorized!\n";
                }
                else {
                    cout << "Book data format is not correct!\n";
                }
            }
            else {
                cout << "Success in adding books!\n";
            }
            continue;
        }

        // Pentru comanda de stergere a unei carti, am format url-ul necesar de accesare 
        // al cartii cu id-ul dorit, de asemenea, am tratat atat eroarea de autorizare cat si 
        // cea in care cartea cautata nu se afla in biblioteca.
        if (command == "delete_book") {

            cout << "id=";
            string id_book;
            cin >> id_book;

            string url = server.delete_book_url + id_book;

            string server_request = compute_delete_request(server.host_ip, url, "", "",
                             current_token, current_cookie);
            
            send_to_server(server_socket, (char *) server_request.c_str());

            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                if (result.find("Authorization") != string::npos) {
                    cout << "Failed to delete book, Not Authorized!\n";
                }
                else {
                    cout << "Book with Id: " << id_book << " doesn't exist\n";
                }
            }
            else {
                cout << "Success in deleting the book with Id: " << id_book << '\n';
            }

            continue;
        }

        // Comanda ce incheie sesiunea curenta prin reactualizarea variabilei de log in pe false si 
        // stergerea cookie-ului si a token-ului Jwt. Astfel, utilizatorul nu mai poate accesa nimic
        // fara sa se conecteze inapoi la server. De asemenea, daca nu suntem logati, vom produce
        // o eroare aferenta.
        if (command == "logout") {

            string server_request = compute_get_request(server.host_ip, server.logout_url, "", current_token, current_cookie);

            send_to_server(server_socket, (char *) server_request.c_str());
            string result = receive_from_server(server_socket);

            if (get_server_results) {
                cout << result << '\n';
            }

            if (result.find("error") != string::npos) {
                cout << "You are not Logged In\n";
            }
            else {
                rights.loged_in_on_server = false;
                current_cookie = "";
                current_token = "";
                cout << "Logout with success!\n";
            }
            continue;

        }  

    }

    return 0;
}