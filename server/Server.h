#ifndef CHARTROOM_SERVER_H
#define CHARTROOM_SERVER_H

#include <string>
#include "Head.h��

using namespace std;

class Server{

public:
	Server();        // ������࣬��������ͻ�������

	void Init();    // �޲������캯��

	void Start();   // ���������

	void Close();   // �رշ����

private:
	int SendBroadcastMessage(int clientfd);   // �㲥��Ϣ�����пͻ���

    struct sockaddr_in serverAddr;       // ��������serverAddr��Ϣ
	
	int listenfd;					//����������socket

	int epfd;					 // epoll_create������ķ���ֵ

	list<int> clients_list;		// �ͻ����б�
};