#include "comm.hpp"
#define PORT 8888

int main(int argc, char *argv[]) {
    CommServer server(PORT, 0);
    while (1) {
        auto rec = server.recv();
        printf("%s\n", rec.get());
        if (rec[0] == '*') {
            break;
        }
    }
    server.recvbig();
    return 0;
}
