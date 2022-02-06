#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "comm.hpp"
#define PORT 8888
int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen;
    char *buffer = (char *)malloc(654990000);
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    CHK_ERR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
    int option = 1;
    CHK_ERR(
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)));

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    CHK_ERR(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    CHK_ERR(newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                               (socklen_t *)&clilen));
    close(sockfd);
    int sun = 0;
    while (1) {
        CHK_ERR(n = recv(newsockfd, buffer, 654990000, 0));
        sun += n;
        if (sun == 654990000) break;
        printf("%d\n", sun);
    }
    send(newsockfd, "ok", 2, 0);

    return 0;
}