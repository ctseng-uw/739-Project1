
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define TIMEOUT 10

#define CHK_ERR(stmt)         \
    if ((stmt) < 0) {         \
        assert_perror(errno); \
    }
template <typename T>
class CircularBuffer {
   protected:
    std::vector<T> v;
    int st, ed;

   public:
    CircularBuffer(int size) {
        v.resize(size);
        st = ed = 0;
    }

    T &operator[](int index) { return v[(st + index) % v.size()]; }
    T &front() { return this->v[this->st]; }
    void pop_front() {
        this->v[this->st] = T();
        this->st = (this->st + 1) % this->v.size();
    }
};

template <typename T>
class CircularQueue : public CircularBuffer<T> {
   private:
    int sz;

   public:
    CircularQueue(int size) : CircularBuffer<T>(size) { sz = 0; }
    bool empty() { return sz == 0; }
    bool full() { return sz == this->v.size(); }
    int size() { return sz; }

    void push_back(T item) {
        assert(this->sz < this->v.size());
        this->v[this->ed] = item;
        this->ed = (this->ed + 1) % this->v.size();
        this->sz++;
    }
    void pop_front() {
        assert(this->sz > 0);
        this->st = (this->st + 1) % this->v.size();
        this->sz--;
    }
};

class Comm {
   public:
    static constexpr int max_data_size = 100 - sizeof(int64_t);
    static const int window_size = 3;

   protected:
    struct packet {
        int64_t seq;
        char data[max_data_size];
    };
    struct ack_packet {
        int64_t ack;
    };
    long get_time_ms() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
};

class CommClient : Comm {
   private:
    int fd;
    struct sockaddr_in addr;
    uint64_t next_seq;
    struct pollfd pollfd;

   public:
    CommClient(std::string hostname, int port) {
        next_seq = 1;
        CHK_ERR(fd = socket(AF_INET, SOCK_DGRAM, 0));
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        struct in_addr *in_addr;
        struct hostent *host_entry;
        host_entry = gethostbyname(hostname.c_str());
        assert(host_entry != NULL);
        in_addr = (struct in_addr *)host_entry->h_addr_list[0];
        addr.sin_addr = *in_addr;

        pollfd.fd = fd;
        pollfd.events =
            POLLIN | POLLERR | POLLPRI | POLLRDHUP | POLLHUP | POLLNVAL;
    }

    void send(const void *data, int len) {
        assert(len <= max_data_size);
        struct packet pkt;
        struct ack_packet ack_pkt;
        pkt.seq = next_seq++;
        memcpy(pkt.data, data, len);
        while (1) {
            CHK_ERR(sendto(fd, &pkt, len + sizeof(int64_t), 0,
                           (struct sockaddr *)&addr, sizeof(addr)));

            int ret = poll(&pollfd, 1, TIMEOUT);
            if (ret == 0) {
                std::cerr << "timeout" << std::endl;
                continue;
            }
            CHK_ERR(recvfrom(fd, &ack_pkt, sizeof(struct ack_packet), 0, NULL,
                             NULL));

            if (ack_pkt.ack == pkt.seq) {
                break;
            }
        }
    }

    void sendbig(void *data, uint64_t len) {
        struct cque_item {
            int64_t seq, st_bytes;
            long ts;
            bool ok;
            int data_len;
        };
        CircularQueue<struct cque_item> cque(window_size);
        int64_t cur_bytes = 0;
        struct packet pkt;
        struct ack_packet ack_pkt;
        while (1) {
            for (int i = 0; i < cque.size(); i++) {
                if (cque[i].ok || get_time_ms() - cque[i].ts < TIMEOUT)
                    continue;
                std::cerr << "resend" << std::endl;
                pkt.seq = cque[i].seq;
                memcpy(pkt.data, (char *)data + cque[i].st_bytes,
                       cque[i].data_len);
                CHK_ERR(sendto(fd, &pkt, cque[i].data_len + sizeof(int64_t), 0,
                               (struct sockaddr *)&addr, sizeof(addr)));
                cque[i].ts = get_time_ms();
            }
            int data_len =
                std::min(len - cur_bytes, static_cast<uint64_t>(max_data_size));
            if (data_len <= 0 && cque.empty()) break;

            if (data_len > 0 && !cque.full()) {
                pkt.seq = next_seq++;
                if (cur_bytes + data_len >= len) pkt.seq = -pkt.seq;
                memcpy(pkt.data, (char *)data + cur_bytes, data_len);
                CHK_ERR(sendto(fd, &pkt, data_len + sizeof(int64_t), 0,
                               (struct sockaddr *)&addr, sizeof(addr)));

                long ts = get_time_ms();
                cque.push_back({abs(pkt.seq), cur_bytes, ts, false, data_len});
                cur_bytes += data_len;
            }

            if (cur_bytes < len && !cque.full()) continue;

            int poll_ret = poll(&pollfd, 1, TIMEOUT);
            if (poll_ret == 0) {
                continue;
            } else if (poll_ret > 0) {
                do {
                    CHK_ERR(recvfrom(fd, &ack_pkt, sizeof(struct ack_packet), 0,
                                     NULL, NULL))
                    int idx = ack_pkt.ack - cque.front().seq;
                    if (idx < 0 || idx >= cque.size()) continue;
                    cque[idx].ok = true;
                } while (poll(&pollfd, 1, 0) > 0);
                while (!cque.empty() && cque.front().ok) {
                    cque.pop_front();
                }
            } else {
                assert_perror(errno);
            }
        }
    }
};

class CommServer : Comm {
   private:
    int fd;
    int drop_rate;
    uint64_t next_seq;

   public:
    CommServer(int port, int drop_rate) {
        srand(time(NULL));
        next_seq = 1;
        this->drop_rate = drop_rate;
        CHK_ERR(fd = socket(AF_INET, SOCK_DGRAM, 0));
        struct sockaddr_in my_addr;
        memset(&my_addr, 0, sizeof(struct sockaddr_in));
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(port);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        CHK_ERR(bind(fd, (struct sockaddr *)&my_addr, sizeof(my_addr)));
    }

    void recv() {
        struct sockaddr remote_addr;
        socklen_t addr_len = sizeof(struct sockaddr);
        struct packet pkt;
        int len;
        while (1) {
            CHK_ERR(len = recvfrom(fd, &pkt, sizeof(struct packet), 0,
                                   &remote_addr, &addr_len))
            pkt.seq = abs(pkt.seq);
            if (pkt.seq < next_seq) {
                std::cerr << "duplicate" << std::endl;
                break;
            } else if (rand() % 100 < drop_rate) {
                std::cerr << "drop" << std::endl;
            } else {
                next_seq = pkt.seq + 1;
                break;
            }
        }
        struct ack_packet ack_pkt;
        ack_pkt.ack = pkt.seq;
        CHK_ERR(sendto(fd, &ack_pkt, sizeof(struct ack_packet), 0, &remote_addr,
                       addr_len));
        write(STDOUT_FILENO, pkt.data, len - sizeof(int64_t));
        return;
    }

    void recvbig() {
        struct cbuf_item {
            struct packet pkt;
            int len;
        };

        CircularBuffer<std::unique_ptr<cbuf_item>> cbuf(window_size);
        int len;
        struct sockaddr remote_addr;
        socklen_t addr_len = sizeof(struct sockaddr);
        struct packet pkt;
        while (1) {
            CHK_ERR(len = recvfrom(fd, &pkt, sizeof(struct packet), 0,
                                   &remote_addr, &addr_len))
            if (rand() % 100 < drop_rate) {
                std::cerr << "drop" << std::endl;
                continue;
            }
            struct ack_packet ack_pkt;
            ack_pkt.ack = abs(pkt.seq);
            CHK_ERR(sendto(fd, &ack_pkt, sizeof(struct ack_packet), 0,
                           &remote_addr, addr_len));

            int idx = abs(pkt.seq) - next_seq;
            if (idx < 0 || idx >= window_size || cbuf[idx] != nullptr) {
                std::cerr << "out of window / duplicate" << std::endl;
                continue;
            }
            cbuf[idx] = std::make_unique<struct cbuf_item>(cbuf_item{pkt, len});

            while (cbuf.front() != nullptr) {
                next_seq++;
                write(STDOUT_FILENO, cbuf.front()->pkt.data,
                      cbuf.front()->len - sizeof(int64_t));
                if (cbuf.front()->pkt.seq < 0) return;
                cbuf.pop_front();
            }
        }
    }
};