#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include "../../docker-deploy/src/server_util.hpp"

using namespace std;
std::string readXMLFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
int main(int argc, char * argv[]) {
    const char *server_hostname = "127.0.0.1"; 
    const char *server_port = "12345"; 
    int socket_fd;

    socket_fd = connect_to_external_server(server_hostname, server_port);
    if (socket_fd == -1) {
        cerr << "Error: failed to connect to the server" << endl;
        return 1;
    }
    string message = readXMLFileToString(argv[1]); 
    int length = message.length();
    //cout<<length<<endl;
    int bytes_sent = send(socket_fd, message.c_str(), message.length(), 0);
    if (bytes_sent == -1) {
        cerr << "Error: failed to send message" << endl;
    } else {
        cout << "*******Message sent to server: \n" << message << endl;
        cout << "===============================================\n"  << endl;
    }
    const int BUFFER_SIZE = 6500;
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        cerr << "Error: Failed to send response to client" << endl;
    }else{
        cout << "*******Message receive response: \n" << buffer << endl;
        cout << "===============================================\n"  << endl;
    }

    // Close socket
    //close(socket_fd);

    return 0;
}
