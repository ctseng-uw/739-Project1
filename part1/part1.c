#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

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

int main(int argc, char *argv[]) {
    struct timespec start, end, diff;
    long int sum;
    int iterations;
    char *filename;
    int ret;

    if (argc < 2) {
        printf("Please specify the number of iterations to average over\n");
        return -1;
    } else if (argc < 3) {
        filename = NULL;
    } else {
        filename = argv[2];
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

    // Measure the time it takes to read 1MB from disk
    if (filename == NULL)
        return 0;
    sum = 0;
    for (int i = 0; i < iterations; i++) {
        char buf[MB];
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
    }

    return 0;
}
