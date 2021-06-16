#ifndef CHARTROOM_CLIENT_H
#define CHARTROOM_CLIENT_H

#include <string>
#include "Common.h"

using namespace std;

class Client {
public:
	
    // �޲������캯��
	Client();


    // ���ӷ�����
	void Connect();


    // �Ͽ�����
	void Close();


    // �����ͻ���
	void Start();
private:

    // ��ǰ���ӷ������˴�����socket
	int sock;

    // ��ǰ����ID
	int pid;


    // epoll_create������ķ���ֵ
	int epfd;

    // �����ܵ�������fd[0]���ڸ����̶���fd[1]�����ӽ���д
	int pipe_fd[2];


    // ��ʾ�ͻ����Ƿ���������
	bool isClientwork;


    // ������Ϣ
	Msg msg;

	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];

    //�û����ӵķ����� IP + port
	struct sockaddr_in serverAddr;

};