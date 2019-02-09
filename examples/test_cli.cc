#include <iostream>
#include <thread>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>
#include <functional>
#include <unistd.h>

class TestClient
{
public:
    TestClient()
    {
        struct timeval timeout = {3,0};
        sprintf(m_buf, "heloo world");
        struct sockaddr_in servaddr;
        if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(2099);
        int on = 1;
        if (connect(m_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
            printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        }
        setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &on, sizeof(on));
        setsockopt(m_sockfd, SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        m_bRecived = true;
    }

    ~TestClient()
    {
        close(m_sockfd);

        m_bRecvStop = false;
        if(m_recvThread.joinable())
            m_recvThread.join();
    }

    void senddata() {
        int count = 0;
        m_bRecvStop = true;
        m_recvThread = std::thread(&TestClient::recvdata, this);

        while (count < 10) {

                int ret = write(m_sockfd, m_buf, strlen(m_buf));
                count++;
                int cc = 0;
                while(cc < 1000)
                    cc++;

        }
    }
    void recvdata()
    {
        while(m_bRecvStop)
        {
            char buf[2048];
            int ret = read(m_sockfd, buf, 2048);
            if(ret <= 0)
                break;
            std::cout << "recv " << ret << " bytes" << std::endl;
        }
    }

private:
    char m_buf[2048];
    std::thread m_recvThread;
    int m_sockfd;
    bool m_bRecvStop;
    bool m_bRecived;
};

int main() {
    TestClient t_client;
    t_client.senddata();
    getchar();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
