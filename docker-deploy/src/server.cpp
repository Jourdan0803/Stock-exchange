#include "server_util.hpp"
#include "parser.hpp"
#include "database_util.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sstream>
#include <thread>
using namespace std;
string receiveMessage(int client_fd){
       const int BUFFER_SIZE = 6500;
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            cerr << "Error: failed to receive data from the client" << endl;
            return "";
        } else if (bytes_received == 0) {
            cerr << "Connection closed by the client" << endl;
            return "";
        } else {
            buffer[bytes_received] = '\n';
            cout<<buffer<<endl;
            stringstream ss(buffer);
            string first_line;
            getline(ss, first_line);
            int xml_data_size = stoi(first_line);
            buffer[bytes_received] = '\0'; 
            string receivedData(buffer);
            if(receivedData.length()!=xml_data_size + 1 +first_line.length()){
                cerr<<"Error: Received length not matched"<<endl;
                return "";
            }else{
                stringstream ss(receivedData);
                string discardLine;
                getline(ss, discardLine);
                getline(ss, discardLine);
                string xmlData((istreambuf_iterator<char>(ss)), istreambuf_iterator<char>());
                //cout << "Received from client: " << xmlData << endl;
                return xmlData;
            }
    }
}
void handleClient(int client_fd, string client_ip){
    connection *c = create_database_connection("passw0rd");
    cout << "Connected to client with IP: " << client_ip << endl;
    string message = receiveMessage(client_fd);
    if(message!=""){
        //cout<<message<<endl;
        string response = handleRequest(message,c);
        int bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
        if (bytes_sent == -1) {
            cerr << "Error: Failed to send response to client" << endl;
        }
    }
    close(client_fd);
    close_database_connection(c);
}
int main() {
    const char *port = "12345";
    // cout << "trying to set up databse ..." << endl;
    while(true){
        try{
            create_database("passw0rd");
            break;
        }
        catch (const std::exception &e){
            cerr << e.what() << std::endl;
            continue;
        }
    }
    // cout << "successfully create databse ... " << endl;
    int server_fd = establish_server(port); 
    if (server_fd == -1) {
        cerr << "Error: Failed to establish server." << endl;
        return 1;
    }
    while (true) {
       string client_ip;
        int client_fd = server_accept_connection(server_fd, &client_ip); 
        if (client_fd == -1) {
            cerr << "Error: Failed to accept client connection." << endl;
            continue;
        }
        
        thread clientThread(handleClient, client_fd, client_ip);
        clientThread.detach();
;
    }
    close(server_fd);
    // close_database_connection(c);
    return 0;
}
