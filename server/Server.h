#ifndef CHARTROOM_SERVER_H
#define CHARTROOM_SERVER_H

#include <string>
#include "Head.h”

using namespace std;

class Server{

public:
	Server();        // 服务端类，用来处理客户端请求

	void Init();    // 无参数构造函数

	void Start();   // 启动服务端

	void Close();   // 关闭服务端

private:
	int SendBroadcastMessage(int clientfd);   // 广播消息给所有客户端

    struct sockaddr_in serverAddr;       // 服务器端serverAddr信息
	
	int listenfd;					//创建监听的socket

	int epfd;					 // epoll_create创建后的返回值

	list<int> clients_list;		// 客户端列表
};