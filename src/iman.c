#include "iman.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int iMan(char** args) {
    if(args == NULL || args[1] == NULL) {
        fprintf(stderr, "Invalid usage of iMan\n");
        return -1;
    }
    char req[1000];
    snprintf(req, sizeof(req), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\nConnection: keep-alive\r\n\r\n", args[1]);

    struct hostent* server = gethostbyname("man.he.net");
    if(server == NULL) {
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
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)server->h_addr_list[0])));

    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Failed to connect\n");
        close(sockfd);
        return -1;
    }
    if(send(sockfd, req, strlen(req), 0) < 0) {
        fprintf(stderr, "Failed to send request\n");
        close(sockfd);
        return -1;
    }
    char response[4096];
    int n;
    while(n = recv(sockfd, response, sizeof(response) - 1, 0) > 0) {
        response[n] = '\0';
        printf("%s", response);
    }
    if(n < 0) {
        fprintf(stderr, "Failed to receive response\n");
        return -1;
    }
    close(sockfd);
    return 0;
}