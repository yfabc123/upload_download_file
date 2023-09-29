#ifndef _FILE_TRANSFER_H__
#define _FILE_TRANSFER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
typedef struct file_protocol
{
    size_t filesize;    // 文件大小
    char filename[256]; // 文件名
    int type;           // 操作类型 0 上传 1下载
} file_protocol_t;

// 以不粘包的方式 接收协议头信息
extern int recv_protocol_head(int cfd, file_protocol_t *p_head);

// 接收数据并保存到本地文件中
extern int recv_filedata(int cfd, const char *filename, size_t targetsize);

// 处理对端上传文件逻辑
extern int client_upload_file(int cfd);

// 发送协议头信息给对端
extern int send_protocol_head(const char *filename, int sockfd, int type);

// 客户端视角
//  上传文件逻辑 send_protocol_head+发送数据逻辑
extern int upload_file(const char *filename, int sockfd);

// 下载文件逻辑
extern int download_file(const char *filename, int sockfd);

// 客户端接收协议头信息

#endif