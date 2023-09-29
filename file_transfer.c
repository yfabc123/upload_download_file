#include "file_transfer.h"
#include "debug.h"
#include "tcpsocket.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int recv_protocol_head(int cfd, file_protocol_t *p_head)
{
    char *buf = (char *)p_head;
    size_t cur_len = 0; // 记录当前接收长度
    size_t rbytes = 0;
    // 循环接收数据
    while (1)
    {
        rbytes = recv(cfd, buf + cur_len, sizeof(file_protocol_t) - cur_len, 0);
        printf("recv_protocol_head rbytes:%ld\n", rbytes);
        if (rbytes == -1)
        {
            DEBUG_INFO("recv:%s", strerror(errno));
            break;
        }
        else if (rbytes == 0)
        {
            DEBUG_INFO("发送端已下线");
            break;
        }
        else if (rbytes > 0)
        {

            cur_len = cur_len + rbytes;
            if (cur_len == sizeof(file_protocol_t))
            {
                break;
            }
        }
    }
    printf("recv_protocol_head 调用成功\n");
    return 0;
}

int recv_filedata(int cfd, const char *filename, size_t targetsize)
{
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);

    char buffer[1024] = {0};
    size_t rbytes = 0, wbytes = 0, cur_size = 0;

    // 循环接收数据同时存放到文件中
    while (1)
    {
        printf("循环中\n");
        rbytes = tcp_recv_pack(cfd, buffer, sizeof(buffer));
        printf("recv_filedata rbytes:%ld\n", rbytes);

        if (rbytes == -1)
        {
            DEBUG_INFO("tcp_recv_pack:%s", strerror(errno));
            break;
        }
        else if (rbytes == 0)
        {
            DEBUG_INFO("发送端下线");
            break;
        }
        else if (rbytes > 0)
        {
            wbytes = write(fd, buffer, rbytes);
            if (wbytes != rbytes)
            {
                DEBUG_INFO("write 有问题 读写数量不一致");
                break;
            }
            cur_size += wbytes;
            if (cur_size == targetsize)
            {
                printf("写入数据到本地文件完成\n");
                break;
            }
        }
    }
    close(fd);
    //   return 0; //为什么不return  因为最好把收到的数据量反馈出去
    printf("recv_filedata 调用成功\n");
    return targetsize;
}

int client_upload_file(int cfd)
{
    file_protocol_t p_head;
    recv_protocol_head(cfd, &p_head); // 把客户端协议头信息保存到我定义的结构体里面

    int size = recv_filedata(cfd, p_head.filename, p_head.filesize);

    return size;
}

int send_protocol_head(const char *filename, int sockfd, int type)
{
    printf("进入send_protocol_head\n");

    file_protocol_t head;

    if (type == 0) // "上传" 协议头
    {
        if (access(filename, F_OK) != 0)
        {

            DEBUG_INFO("文件不存在");
            exit(EXIT_FAILURE);
        }

        int fd = open(filename, O_RDONLY);
        if (fd == -1)
        {
            DEBUG_INFO("open:%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        size_t size = lseek(fd, 0, SEEK_END); // 得到文件大小
        close(fd);                            // 注意操作完文件之后关闭文件
        head.filesize = size;
        head.type = type;
        strcpy(head.filename, filename);
    }
    else if (type == 1) // "下载" 协议头
    {
        int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC);
        if (fd == -1)
        {
            DEBUG_INFO("open:%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        head.filesize = 0;
        head.type = type;
        strcpy(head.filename, filename);
    }

    int ret = tcp_send_pack(sockfd, &head, sizeof(head)); // 返回值和send一样 即实际发送数
    if (ret != sizeof(head))
    {
        DEBUG_INFO("tcp_send_pack:%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0; // 上传和下载 结果不一样
}

int upload_file(const char *filename, int sockfd)
{
    if (access(filename, F_OK) != 0)
    {
        DEBUG_INFO("文件不存在");
        return -1;
    }

    // 1.协议头信息发送给服务器
    size_t file_size; // 要发送数据的字节数
    file_size = send_protocol_head(filename, sockfd, 0);
    if (file_size < 0)
    {
        DEBUG_INFO("send_protocol_head");
        exit(EXIT_FAILURE);
    }

    int fd = open(filename, O_RDONLY);
    printf("debug:%d\n", fd);

    char buffer[1024] = {0};
    size_t rbytes = 0; // 读取字节
    size_t sbytes = 0; // 发送字节

    size_t upload_size = 0; // 当前上传字节
    // 循环读取发送
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("已进入upload_file循环读取数据到缓冲区然后发送逻辑\n");
        rbytes = read(fd, buffer, sizeof(buffer));
        printf("rbytes:%ld\n", rbytes);
        if (rbytes < 0)
        {
            DEBUG_INFO("read:%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if (rbytes == 0) // 表示文件内容已全部读取可退出
        {
            break;
        }

        sbytes = tcp_send_pack(sockfd, buffer, rbytes);
        if (sbytes != rbytes)
        {
            DEBUG_INFO("tcp_send_pack 发送数据和读取数据不一致");
            exit(EXIT_FAILURE);
        }
        upload_size += sbytes; // 更新上传字节数
        sleep(1);
    }
    close(fd);
    printf("upload_file 调用成功\n");
    return upload_size;
}

int download_file(const char *filename, int sockfd)
{
    // 1.告知对端我要下载
    send_protocol_head(filename, sockfd, 1);
    // 复用服务器接收数据逻辑
    client_upload_file(sockfd);
    return 0;
}