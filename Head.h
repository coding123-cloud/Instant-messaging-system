#ifndef CHARTROOM_HEAD_H
#define CHARTROOM_HEAD_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

#define EPOLL_SIZE 5000  //EPOLL֧���������
#define BUFF_SIZE 65535


// ���û���¼��Ļ�ӭ��Ϣ
#define SERVER_LOGIN_INFO "Welcome you join to the chat room! Your chat ID is: Client #%d"
	

// �����û��յ���Ϣ��ǰ׺
#define SERVER_MESSAGE "ClientID %d say >> %s"
#define SERVER_PRIVATE_MESSAGE "Client %d say to you privately >> %s"
#define SERVER_PRIVATE_ERROR_MESSAGE "Client %d is not in the chat room"

// �˳�ϵͳ
#define EXIT "EXIT"
#define CAUTION "There is only one int the char room!"

// ����socketΪ������ģʽ
// �׽������̷��أ�����I/O�Ƿ���ɣ��ú������ڵ��̻߳������
//eg. ��recv(fd...)ʱ���ú������̷��أ��ڷ���ʱ���ں����ݻ�û׼���û᷵��WSAEWOULDBLOCK�������
static void addfd (int epfd, int fd, bool enable_et) {
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if (enable_et)
	{
		ev.events = EPOLLIN | EPOLLET;
	}
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
	printf("fd added to epollfd!\n\n");
}

struct Msg
{
	int type;
	int fromID;
	int toID;
	char content_Buffer[DUF_SIZE];
};

#endif;