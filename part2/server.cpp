#include "comm.hpp"
#define PORT 8888

int main(int argc, char *argv[]) {
    CommServer server(PORT, atoi(argv[2]));
    if (argv[1][0] == '1' || argv[1][0] == '2' || argv[1][0] == '4') {
        while (1) {
            server.recv();
        }
    } else if (argv[1][0] == '3') {
        server.recvbig();
    }

    return 0;
}
