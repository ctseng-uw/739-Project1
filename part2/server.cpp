#include "comm.hpp"
#define PORT 8888

int main(int argc, char *argv[]) {
    CommServer server(PORT, 50);
    while (1) {
        printf("%s\n", server.recv().get());
    }

    return 0;
}