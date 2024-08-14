#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <cstring>
int establish_server(const char * port);
int server_accept_connection(int our_server_fd, std::string * client_ip);
int connect_to_external_server(const char *hostname, const char *port);

#endif