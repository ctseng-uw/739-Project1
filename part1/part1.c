#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MB 1024*1024

struct timespec time_difference(struct timespec start, struct timespec end) {
    struct timespec diff;

    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_nsec = end.tv_nsec - start.tv_nsec;

    if (diff.tv_nsec < 0) {
        diff.tv_nsec += 1000000000;
        diff.tv_sec--;
    }

    return diff;
}

int connect_socket(char *ip) {
    int sock;
    struct sockaddr_in remote = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    remote.sin_addr.s_addr = inet_addr(ip);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(1234);

    connect(sock, (struct sockaddr *)&remote, sizeof(struct sockaddr_in));

    return sock;
}

int main(int argc, char *argv[]) {
    struct timespec start, end, diff;
    long int sum;
    int iterations;
    char *filename = NULL;
    char *ip = NULL;
    int ret;
    char buf[MB];
    char buf2[MB];
    int sock;

    if (argc < 2) {
        printf("Please specify the number of iterations to average over\n");
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
        clock_gettime(CLOCK_REALTIME, &start);
        clock_gettime(CLOCK_REALTIME, &end);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average clock_gettime %ld\n", sum / iterations);

    // Measure the time it takes to access main memory
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        long t1 = 10;
        long t2 = 5;
        long *ref = &t2;

        clock_gettime(CLOCK_REALTIME, &start);
        asm("MOVNTI %0, (%1); MFENCE"
            : /* No output registers*/
            : "r" (t1), "r" (ref)
            : /* No clobbers */
        );
        clock_gettime(CLOCK_REALTIME, &end);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average main memory access store %ld\n", sum / iterations);

    // Measure the time it takes to lock a mutex
    sum = 0;
    pthread_mutex_t lock;
    for (int i = 0; i < iterations; i++) {
        clock_gettime(CLOCK_REALTIME, &start);
        pthread_mutex_lock(&lock);
        clock_gettime(CLOCK_REALTIME, &end);

        pthread_mutex_unlock(&lock);
        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average mutex lock time %ld\n", sum / iterations);

    // Measure the time it takes to read 1MB from memory
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        clock_gettime(CLOCK_REALTIME, &start);
        memcpy(buf2, buf, MB);
        clock_gettime(CLOCK_REALTIME, &end);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
        buf[0] = buf2[0];
    }
    printf("average memory read %ld\n", sum / iterations);

    // Measure the time it takes to read 1MB from disk
    if (filename == NULL)
        return 0;
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        int fd = open(filename, O_RDONLY);

        // Clear the page cache
        ret = system("sync; echo 3 > /proc/sys/vm/drop_caches");

        clock_gettime(CLOCK_REALTIME, &start);
        ret = read(fd, buf, MB);
        clock_gettime(CLOCK_REALTIME, &end);

        if (ret != MB) {
            printf("Error!");
            return -1;
        }

        close(fd);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average disk sequential read %ld\n", sum / iterations);

    // Read 4K randomly from disk
    sum = 0;
    for (int i = 0; i < iterations; i ++) {
        int increment = (4 * 1024) / 16;
        int fd = open(filename, O_RDONLY);
        // Clear the page cache
        ret = system("sync; echo 3 > /proc/sys/vm/drop_caches");

        clock_gettime(CLOCK_REALTIME, &start);
        for (int j = 0; j < 16; j++) {
            int index = rand() % 16;
            lseek(fd, increment * index, SEEK_SET);
            read(fd, buf, increment);
        }
        clock_gettime(CLOCK_REALTIME, &end);

        close(fd);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average random read %ld\n", sum / iterations);

    // Measure the time it takes to send 10K over network link
    if (ip == NULL)
        return 0;
    sock = connect_socket(ip);
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        clock_gettime(CLOCK_REALTIME, &start);
        send(sock, buf, 1*1024, 0);
        clock_gettime(CLOCK_REALTIME, &end);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average network send %ld\n", sum /iterations);

    return 0;
}
