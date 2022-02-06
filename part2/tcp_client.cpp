#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <chrono>

#include "comm.hpp"

#define HOST "localhost"
#define PORT 8888

int main(int argc, char *argv[]) {
    using namespace std::chrono;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    CHK_ERR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
    server = gethostbyname(HOST);
    assert(server != NULL);

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(PORT);
    CHK_ERR(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))

    using namespace std;
    using namespace std::chrono;
    int loop_cnt = 10000;

    char *large = new char[Comm::max_data_size * loop_cnt];
    for (int i = 0; i < Comm::max_data_size * loop_cnt; i++) {
        large[i] = '0' + i % 10;
    }

    char buffer[256];
    auto t1 = high_resolution_clock::now();
    send(sockfd, large, Comm::max_data_size * loop_cnt, 0);
    recv(sockfd, buffer, 256, 0);
    auto t2 = high_resolution_clock::now();
    auto span = duration_cast<milliseconds>(t2 - t1);
    auto size = static_cast<uint64_t>(Comm::max_data_size) * loop_cnt;
    cout << "sent " << size << " bytes (" << size / 1024 / 1024
         << " MB) with sliding window in " << span.count() << " ms" << endl;

    return 0;
}