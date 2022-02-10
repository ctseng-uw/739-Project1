#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>

int main(int argc, char *argv[]) {
    long int sum;
    int iterations;
    int averages;

    if (argc < 2) {
        std::cout << "Specify number of iterations to average over\n";
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
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }
    std::cout << "average clock_gettime " <<  sum / averages << std::endl;

    sum = 0;
    for (int i = 0; i < averages; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iterations; j++) {
            asm("nop");
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = end - start;
        sum += diff.count();
    }

    std::cout << "average time " << sum /averages << std::endl;

    return 0;
}
