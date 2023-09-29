#include "../tcpsocket.h"
#include "../file_transfer.h"

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        printf("参数少了\n");
        exit(EXIT_FAILURE);
    }
    int cli_socket = create_tcp_client_socket(argv[1], atoi(argv[2]));

    // main函数获取参数 第5个参数是0 表示上传 1表示下载
    if (atoi(argv[4]) == 0) // 上传文件
    {

        upload_file(argv[3], cli_socket);
        close(cli_socket);
    }

    else if (atoi(argv[4]) == 1)
    {
        download_file(argv[3],cli_socket);
        close(cli_socket);
    }

    return 0;
}
