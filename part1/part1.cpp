#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <chrono>
#include <iostream>
#include "snappy/snappy-c.h"

#define MB 1024*1024
#define GB 1024*1024*1024
#define L1_SIZE ((128 * 1024) / sizeof(long))

int connect_socket(char *ip) {
    int sock;
    struct addrinfo hints, *res, *p;
    struct sockaddr_in remote = {0};

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(ip, "1234", &hints, &res);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    remote.sin_addr.s_addr = inet_addr(ip);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(1234);

    connect(sock, (struct sockaddr *)res->ai_addr, sizeof(struct sockaddr_in));

    return sock;
}

int main(int argc, char *argv[]) {
    long int sum;
    int iterations;
    char *filename = NULL;
    char *ip = NULL;
    int ret;
    char buf[MB];
    char buf2[MB];
    int sock;

    if (argc < 2) {
        std::cout << "Please specify the number of iterations to average over\n";
        return -1;
    }
    if (argc >= 3) {
        filename = argv[2];
    }
    if (argc >= 4) {
        ip = argv[3];
    }
    iterations = atoi(argv[1]);

    // Measure the time taken from the clock_gettime calls
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average high_resolution_clock::now() time " << sum / iterations << std::endl;

    // Measure MFENCE time
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        asm("MFENCE");
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "MFENCE time " << sum / iterations << std::endl;

    // Measure L1 cache hit latency
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        long t1 = 10;
        long t2 = 5;
        long *ref = &t2;

        // Put the memory address in the cache
        asm("MOV %0, (%1); MFENCE"
            : /* No output registers */
            : "r" (t1), "r" (ref)
            : /* No clobbers */
        );

        auto start = std::chrono::high_resolution_clock::now();
        asm("MOV %0, (%1); MFENCE"
            : /* No output registers */
            : "r" (t1), "r" (ref)
            : /* No clobbers */
        );
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "l1 cache hit time " << sum / iterations << std::endl;

    // Measure L2 cache hit latency
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        long t1 = 10;
        long t2 = 5;
        long *ref = &t2;
        // Have it be double the cache size because of cache lines or whatever
        long l1buf[L1_SIZE * 2];

        // Put the memory address in the cache
        asm("MOV %0, (%1); MFENCE"
            : /* No output registers */
            : "r" (t1), "r" (ref)
            : /* No clobbers */
        );

        // Make enough references to put the target memory address outside the cache
        for (int j = 0; j < L1_SIZE * 2; j++) {
            asm("MOV %0, (%1)"
                :
                : "r" (t1), "r" (&l1buf[j])
                :
            );
        }
        asm("MFENCE");

        auto start = std::chrono::high_resolution_clock::now();
        asm("MOV %0, (%1); MFENCE"
            : /* No output registers*/
            : "r" (t1), "r" (ref)
            : /* No clobbers */
        );
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "l2 cache hit time " << sum / iterations << std::endl;

    // Measure the time it takes to access main memory
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        long t1 = 10;
        long t2 = 5;
        long *ref = &t2;

        auto start = std::chrono::high_resolution_clock::now();
        asm("MOVNTI %0, (%1); MFENCE"
            : /* No output registers*/
            : "r" (t1), "r" (ref)
            : /* No clobbers */
        );
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average main memory access store " << sum / iterations << std::endl;

    // Measure the time it takes to lock a mutex
    sum = 0;
    pthread_mutex_t lock;
    for (int i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        pthread_mutex_lock(&lock);
        pthread_mutex_unlock(&lock);
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average mutex lock time " << sum / iterations << std::endl;

    // Measure the time it takes to read 1MB from memory
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        memcpy(buf2, buf, MB);
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average memory read " << sum / iterations << std::endl;

    // Measure the time it take snappy to compress 1k
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        size_t buf_size = MB;
        int fd = open("/dev/urandom", O_RDONLY);
        read(fd, buf, 1024);

        auto start = std::chrono::high_resolution_clock::now();
        snappy_compress(buf, 1024, buf2, &buf_size);
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();

        close(fd);
    }
    std::cout << "average snappy compress time " << sum / iterations << std::endl;

    // Measure the time it takes to read 1MB from disk
    if (filename == NULL)
        return 0;
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        int fd = open(filename, O_RDONLY);

        // Clear the page cache
        ret = system("sync; echo 3 > /proc/sys/vm/drop_caches");

        auto start = std::chrono::high_resolution_clock::now();
        ret = read(fd, buf, MB);
        auto end = std::chrono::high_resolution_clock::now();

        if (ret != MB) {
            printf("Error!");
            return -1;
        }

        close(fd);

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average disk sequential read " << sum / iterations << std::endl;

    // Read 4K randomly from disk
    sum = 0;
    for (int i = 0; i < iterations; i ++) {
        int increment = (4 * 1024) / 16;
        int fd = open(filename, O_RDONLY);
        // Clear the page cache
        ret = system("sync; echo 3 > /proc/sys/vm/drop_caches");

        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < 16; j++) {
            int index = rand() % (GB / increment);
            lseek(fd, increment * index, SEEK_SET);
            read(fd, buf, increment);
        }
        auto end = std::chrono::high_resolution_clock::now();

        close(fd);

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average random read " << sum / iterations << std::endl;

    // Measure the time it takes to send 10K over network link
    if (ip == NULL)
        return 0;
    sock = connect_socket(ip);
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        send(sock, buf, 1*1024, 0);
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average network send " << sum /iterations << std::endl;

    return 0;
}
