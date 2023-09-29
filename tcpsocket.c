
#include "tcpsocket.h"
#include "debug.h"
// 服务端 socket+listen
int create_tcp_server_socket(const char *ip, unsigned short port)
{

    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id < 0)
    {
        DEBUG_INFO("socket:%s", strerror(errno));
        perror("");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in ser_addr;
    int addr_len = sizeof(ser_addr);
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(ip);
    ser_addr.sin_port = htons(port);
    int ret;
    ret = bind(socket_id, (struct sockaddr *)&ser_addr, addr_len);
    if (ret != 0)
    {
        perror("");
        exit(EXIT_FAILURE);
    }

    ret = listen(socket_id, 10); // 设置 socket为监听状态同时创建监听队列 也就是监听做多n个请求
    if (ret != 0)
    {
        perror("");
        exit(EXIT_FAILURE);
    }
    return socket_id;
}

// 客户端 socket+connect
int create_tcp_client_socket(const char *ip, unsigned short port)
{

    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id < 0)
    {
        DEBUG_INFO("socket:%s", strerror(errno));
    }
    struct sockaddr_in ser_addr;
    int addr_len = sizeof(ser_addr);
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(ip);
    ser_addr.sin_port = htons(port);
    int ret;
    ret = connect(socket_id, (struct sockaddr *)&ser_addr, addr_len);
    if (ret != 0)
    {
        DEBUG_INFO("connect:%s", strerror(errno));
    }

    return socket_id;
}
//服务器 accpet
int wait_for_connect(int sockfd, struct sockaddr_in *cli_addr)
{
    int addr_len = sizeof(struct sockaddr_in);
    int new_socket = accept(sockfd, (struct sockaddr *)cli_addr, &addr_len);
    if (new_socket < 0)
    {
        DEBUG_INFO("accpet:%s", strerror(errno));
    }
    return new_socket;
}

void show_tcp_network_address(struct sockaddr_in *sockaddr)
{
    printf("ip : %s\n", inet_ntoa(sockaddr->sin_addr));
    printf("port : %d\n", ntohs(sockaddr->sin_port));
}

ssize_t tcp_send_pack(int sockfd, const void *buf, size_t len)
{
    return send(sockfd, buf, len, 0);
}

ssize_t tcp_recv_pack(int sockfd, void *buf, size_t len)
{
    return recv(sockfd, buf, len, 0);
}
