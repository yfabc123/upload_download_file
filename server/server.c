#include "../tcpsocket.h"
#include "../file_transfer.h"
#include "../debug.h"
#include <pthread.h>

// 我想实现根据参数内容确定是上传还是下载

// 传递给子线程的参数结构体
typedef struct para
{
    int socket_id;
    struct sockaddr_in cli_addr;
    char filename[256];
    size_t filesize; // 文件大小
} para_t;

// 处理客户端上传逻辑
void upload_client(void *arg)
{
    printf("客户端上传文件功能\n");
    para_t para_1 = *(para_t *)arg;
    show_tcp_network_address(&para_1.cli_addr);
    recv_filedata(para_1.socket_id, para_1.filename, para_1.filesize);
    close(para_1.socket_id);
    pthread_exit(NULL);
}
// 处理客户端下载逻辑

void download_client(void *arg)
{
    printf("客户端下载文件功能\n");

    para_t para_1 = *(para_t *)arg;

    upload_file(para_1.filename, para_1.socket_id);
    close(para_1.socket_id);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("参数少了\n");
        exit(EXIT_FAILURE);
    }
    int ser_socket = create_tcp_server_socket(argv[1], atoi(argv[2]));
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    para_t para_thread; // 存放子线程参数结构体
    // 多线程服务器
    int new_socket;
    pthread_t tid;
    while (1)
    {
        new_socket = wait_for_connect(ser_socket, &cli_addr);

        para_thread.socket_id = new_socket; // 的到新socket
        para_thread.cli_addr = cli_addr;    // 得到客户端地址
        file_protocol_t p_head;
        recv_protocol_head(new_socket, &p_head);       // 接收客户端协议头信息
        strcpy(para_thread.filename, p_head.filename); // 得到文件名
        para_thread.filesize = p_head.filesize;
        int ret;
        if (p_head.type == 0)
        {
            ret = pthread_create(&tid, NULL, (void *)upload_client, (void *)&para_thread);
        }
        else if (p_head.type == 1)
        {

            ret = pthread_create(&tid, NULL, (void *)download_client, (void *)&para_thread);
        }

        if (ret != 0)
        {
            DEBUG_INFO("pthread_create:%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        pthread_detach(tid);
    }

    return 0;
}
