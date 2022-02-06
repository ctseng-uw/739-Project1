#include <iostream>
#include <vector>

#include "comm.hpp"

int main(int argc, char *argv[]) {
    using namespace std;
    using namespace std::chrono;
    CommClient client("localhost", 8888);

    uint64_t sum = 0;
    int loop_cnt = 10000;
    char msg[] = "Hello, world!";
    for (int i = 0; i < loop_cnt; i++) {
        auto t1 = high_resolution_clock::now();
        client.send(msg, sizeof(msg));
        auto t2 = high_resolution_clock::now();
        auto span = duration_cast<microseconds>(t2 - t1);
        sum += span.count();
    }
    cout << endl << "round trip avg: " << sum / loop_cnt << " us" << endl;

    char chunck[Comm::max_data_size];
    for (int i = 0; i < Comm::max_data_size; i++) {
        chunck[i] = '0' + i % 10;
    }

    auto t1 = high_resolution_clock::now();
    for (int i = 0; i < loop_cnt; i++) {
        client.send(chunck, Comm::max_data_size);
    }
    auto t2 = high_resolution_clock::now();
    auto span = duration_cast<milliseconds>(t2 - t1);
    auto size = static_cast<uint64_t>(Comm::max_data_size) * loop_cnt;
    cout << "sent " << size << " bytes (" << size / 1024 / 1024 << " MB) in "
         << span.count() << " ms" << endl;

    client.send("*", 1);
    char *large = new char[Comm::max_data_size * loop_cnt];
    for (int i = 0; i < Comm::max_data_size * loop_cnt; i++) {
        large[i] = '0' + i % 10;
    }
    t1 = high_resolution_clock::now();
    client.sendbig(large, Comm::max_data_size * loop_cnt);
    t2 = high_resolution_clock::now();
    span = duration_cast<milliseconds>(t2 - t1);
    cout << "sent " << size << " bytes (" << size / 1024 / 1024
         << " MB) with sliding window in " << span.count() << " ms" << endl;

    return 0;
}