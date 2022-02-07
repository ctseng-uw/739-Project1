# Part 2
## To Compile
```bash
> make
> make O3 # for optimized version
```
## Usage
Please launch client and server with the same test_suite_number
```bash
> ./client test_suite_number hostname
> ./server test_suite_number drop_rate
```
### Example
```bash
> ./client 1 localhost > /dev/null  # redirect output when benchmarking
> ./server 1 0
```

## Configurable compile time paramenters
- `comm.hpp`
    - `TIMEOUT`: Timeout before resending packets (ms)
    - `MAX_UDP_DATA_SIZE`: Max data size of a udp packet. (byte)
        - (Note that our implementation require an int_64t header therefore the true data in a packet will be 8 byte less)
    - `WINDOW_SIZE`: The size of the window buffer (# of packets)
- `client.cpp`
    - `loop_cnt`: control intensity of the test (see below)

## Test Suite
1. Client sent `loop_cnt` numbers of small packets to the server and measure round trip time
2. Client sent `loop_cnt` numbers of of packet each with `MAX_UDP_DATA_SIZE` bytes and measure total time
3. Client sent `loop_cnt` numbers of of packet each with `MAX_UDP_DATA_SIZE` bytes and measure total time but with sliding window optimzation enabled
4. Simple echo server