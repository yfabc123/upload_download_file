#ifndef _DEBUG_H_
#define _DEBUG_H_

/*
	__FILE__ : 输出文件名
	__FUNCTION__ : 输出函数名
	__LINE__ : 输出行号
*/

/* 定义一个缓冲区 将外部报错信息复制到缓冲区 然后打印报错位置信息
 */

#define DEBUG_INFO(args...)                                                           \
	do                                                                                \
	{                                                                                 \
		char b__[1024];                                                               \
		sprintf(b__, args);                                                           \
		fprintf(stderr, "[%s,%s,%d] : %s\n", __FILE__, __FUNCTION__, __LINE__, b__); \
	} while (0)

#endif
