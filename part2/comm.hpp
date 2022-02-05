
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <string>

#define CHK_ERR(stat)         \
    if ((stat) < 0) {         \
        assert_perror(errno); \
    }

class Comm {
   protected:
    static constexpr int max_data_size = 65507 - sizeof(uint64_t);
    struct packet {
        uint64_t seq;
        char data[max_data_size];
    };
    struct ack_packet {
        uint64_t ack;
    };
};

class CommClient : Comm {
   private:
    int fd;
    struct sockaddr_in addr;
    uint64_t next_seq;

   public:
    CommClient(std::string hostname, int port) {
        next_seq = 0;
        CHK_ERR(fd = socket(AF_INET, SOCK_DGRAM, 0));
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        CHK_ERR(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)));
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        struct in_addr *in_addr;
        struct hostent *host_entry;
        host_entry = gethostbyname(hostname.c_str());
        assert(host_entry != NULL);
        in_addr = (struct in_addr *)host_entry->h_addr_list[0];
        addr.sin_addr = *in_addr;
    }

    void send(void *data, int len) {
        assert(len <= max_data_size);
        struct packet pkt;
        struct ack_packet ack_pkt;
        pkt.seq = next_seq++;
        memcpy(pkt.data, data, len);
        while (1) {
            CHK_ERR(sendto(fd, &pkt, len + sizeof(uint64_t), 0,
                           (struct sockaddr *)&addr, sizeof(addr)));
            int ret = recvfrom(fd, &ack_pkt, sizeof(struct ack_packet), 0, NULL,
                               NULL);
            if (ret != -1) {
                break;
            } else if (errno == EAGAIN || ack_pkt.ack != pkt.seq) {
                continue;
            }
            assert_perror(errno);
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
        next_seq = 0;
        this->drop_rate = drop_rate;
        CHK_ERR(fd = socket(AF_INET, SOCK_DGRAM, 0));
        struct sockaddr_in my_addr;
        memset(&my_addr, 0, sizeof(struct sockaddr_in));
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(port);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        CHK_ERR(bind(fd, (struct sockaddr *)&my_addr, sizeof(my_addr)));
    }

    std::unique_ptr<unsigned char[]> recv() {
        struct sockaddr remote_addr;
        socklen_t addr_len = sizeof(struct sockaddr);
        struct packet pkt;
        int len;
        while (1) {
            CHK_ERR(len = recvfrom(fd, &pkt, sizeof(struct packet), 0,
                                   &remote_addr, &addr_len))
            if (pkt.seq < next_seq) {
                puts("duplicate");
            } else if (rand() % 100 < drop_rate) {
                puts("drop");
            } else {
                next_seq = pkt.seq + 1;
                break;
            }
        }
        struct ack_packet ack_pkt;
        ack_pkt.ack = pkt.seq;
        CHK_ERR(sendto(fd, &ack_pkt, sizeof(struct ack_packet), 0, &remote_addr,
                       addr_len));
        auto data = std::make_unique<unsigned char[]>(len - sizeof(uint64_t));
        memcpy(data.get(), pkt.data, len - sizeof(uint64_t));
        return data;
    }
};