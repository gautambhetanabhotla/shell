#include "iman.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int iMan(char** args, FILE* istream, FILE* ostream) {
    if(args == NULL || args[1] == NULL) {
        fprintf(stderr, "Invalid usage of iMan\n");
        return -1;
    }
    char req[1000];
    snprintf(req, sizeof(req), "http://man.he.net/?topic=%s&section=all", args[1]);

    struct hostent* server = gethostbyname("man.he.net");
    if (server == NULL) {
        fprintf(stderr, "No such host\n");
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)server->h_addr)));
}