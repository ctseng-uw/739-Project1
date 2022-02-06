#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    int averages;

    if (argc < 2) {
        printf("Specify number of iterations to average over\n");
        return -1;
    } else if (argc < 3) {
        averages = 10;
    } else {
        averages = atoi(argv[2]);
    }
    iterations = atoi(argv[1]);

    // Measure the time taken from the clock_gettime calls
    sum = 0;
    for (int i = 0; i < averages; i++) {
        clock_gettime(CLOCK_REALTIME, &start);
        clock_gettime(CLOCK_REALTIME, &end);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }
    printf("average clock_gettime %ld\n", sum / averages);

    sum = 0;
    for (int i = 0; i < averages; i++) {
        clock_gettime(CLOCK_REALTIME, &start);
        for (int j = 0; j < iterations; j++) {
            asm("nop");
        }
        clock_gettime(CLOCK_REALTIME, &end);

        diff = time_difference(start, end);
        sum += diff.tv_nsec;
    }

    printf("average time %ld\n", sum /averages);

    return 0;
}
