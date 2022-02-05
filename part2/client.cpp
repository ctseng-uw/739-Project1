#include "comm.hpp"

int main(int argc, char *argv[]) {
    CommClient client("localhost", 8888);
    while (1) {
        char buf[1024];
        scanf("%s", buf);
        client.send(buf, strlen(buf) + 1);
        puts(buf);
    }
    return 0;
}