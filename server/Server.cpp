#include <iostream>
#include "Server.h"

using namespace std;


// ������๹�캯��
Server::Server(){
	
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	listen = 0;

	epfd = 0;

}


// ��ʼ������˲���������
void Server::Init() {
	cout << "Init Server....." <<endl;

	listenfd = socket(PF_INET, SOCK_STREAM, 0);     //��������socket
	if (listen < 0){
		perror("listen error");
		exit(-1);
	}

    //�󶨵�ַ
	if (bind(listenfd, (struct sockaddr *)&serverAdrr, sizeof(serverAdrr)) < 0){
		perror("bind error");
		exit(-1);
	}

	//����
	int ret = listen(listenfd, 5);
	if (ret < 0)
	{
		perror("listen error");
		exit(-1);
	}
	
	cout << "start to listen: " << SERVER_IP << endl;


    //���ں��д����¼��� epfd��һ����� 
	epfd = epoll_create(EPOLL_SIZE);
	if (epfd < 0)
	{
		perror("epfd error");
		exit(-1);
	}

    //���¼�������Ӽ�����
	addfd(epfd, listenfd, true);

}

// �رշ��������ر��ļ�������
void Server::Close() {
 
    //�ر�socket
    close(listenfd);
    
    //�ر�epoll����
    close(epfd);
}
 
// ���͹㲥��Ϣ�����пͻ���
int Server::SendBroadcastMessage(int clientfd)
{
    // buf[BUF_SIZE] ��������Ϣ
    // message[BUF_SIZE] �����ʽ������Ϣ
    char recv_buf[BUFF_SIZE];
    char send_buf[BUFF_SIZE];
    Msg msg;
    bzero(recv_buf, BUFF_SIZE);
    // ��������Ϣ
    cout << "read from client(clientID = " << clientfd << ")" << endl;
    int len = recv(clientfd, recv_buf, BUFF_SIZE, 0);
    //��սṹ�壬�ѽ��ܵ����ַ���ת��Ϊ�ṹ��
    memset(&msg,0,sizeof(msg));
    memcpy(&msg,recv_buf,sizeof(msg));
    //�жϽ��ܵ�����Ϣ��˽�Ļ���Ⱥ��
    msg.fromID=clientfd;
    if(msg.content[0]=='\\'&&isdigit(msg.content[1])){
        msg.type=1;
        msg.toID=msg.content[1]-'0';
        memcpy(msg.content,msg.content+2,sizeof(msg.content));
    }
    else
        msg.type=0;
    // ����ͻ��˹ر�������
    if(len == 0) 
    {
        close(clientfd);
        
        // �ڿͻ����б���ɾ���ÿͻ���
        clients_list.remove(clientfd);
        cout << "ClientID = " << clientfd 
             << " closed.\n now there are " 
             << clients_list.size()
             << " client in the char room"
             << endl;
 
    }
    // ���͹㲥��Ϣ�����пͻ���
    else 
    {
        // �ж��Ƿ������һ��������ͻ���
        if(clients_list.size() == 1){ 
            // ������ʾ��Ϣ
            memcpy(&msg.content,CAUTION,sizeof(msg.content));
            bzero(send_buf, BUFF_SIZE);
            memcpy(send_buf,&msg,sizeof(msg));
            send(clientfd, send_buf, sizeof(send_buf), 0);
            return len;
        }
        //��Ÿ�ʽ�������Ϣ
        char format_message[BUFF_SIZE];
        //Ⱥ��
        if(msg.type==0){
            // ��ʽ�����͵���Ϣ���� #define SERVER_MESSAGE "ClientID %d say >> %s"
            sprintf(format_message, SERVER_MESSAGE, clientfd, msg.content);
            memcpy(msg.content,format_message,BUFF_SIZE);
            // �����ͻ����б����η�����Ϣ����Ҫ�жϲ�Ҫ����Դ�ͻ��˷�
            list<int>::iterator it;
            for(it = clients_list.begin(); it != clients_list.end(); ++it) {
               if(*it != clientfd){
                    //�ѷ��͵Ľṹ��ת��Ϊ�ַ���
                    bzero(send_buf, BUFF_SIZE);
                    memcpy(send_buf,&msg,sizeof(msg));
                    if( send(*it,send_buf, sizeof(send_buf), 0) < 0 ) {
                        return -1;
                    }
               }
            }
        }
        //˽��
        if(msg.type==1){
            bool private_offline=true;
            sprintf(format_message, SERVER_PRIVATE_MESSAGE, clientfd, msg.content);
            memcpy(msg.content,format_message,BUFF_SIZE);
            // �����ͻ����б����η�����Ϣ����Ҫ�жϲ�Ҫ����Դ�ͻ��˷�
            list<int>::iterator it;
            for(it = clients_list.begin(); it != clients_list.end(); ++it) {
               if(*it == msg.toID){
                    private_offline=false;
                    //�ѷ��͵Ľṹ��ת��Ϊ�ַ���
                    bzero(send_buf, BUFF_SIZE);
                    memcpy(send_buf,&msg,sizeof(msg));
                    if( send(*it,send_buf, sizeof(send_buf), 0) < 0 ) {
                        return -1;
                    }
               }
            }
            //���˽�Ķ�������
            if(private_offline){
                sprintf(format_message,SERVER_PRIVATE_ERROR_MESSAGE,msg.toID);
                memcpy(msg.content,format_message,BUFF_SIZE);
                bzero(send_buf,BUFF_SIZE);
                memcpy(send_buf,&msg,sizeof(msg));
                if(send(msg.fromID,send_buf,sizeof(send_buf),0)<0)
                    return -1;
            }
        }
    }
    return len;
}



// ���������

void Server::Start() {
	
    // epoll �¼�����
	static struct epoll_event events[EPOLL_SIZE];
	
	
    // ��ʼ�������
	Init();

	//��ѭ��
	while(1) {
	
        //epoll_events_count��ʾ�����¼�����Ŀ
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1) ;
		if (epoll_events_count < 0)
		{
			perror("epoll error");
			break;
		}

		cout << "epoll_events_count = " << epoll_events_count <<endl;

		//������epoll_events_count�������¼�

		for (int i = 0; i < epoll_events_count ; i++)
		{
			int   sockfd = events[i].data.fd ;
			if (sockfd == listen)  //listen������¼������µĿͻ��˷�����������
			{
				struct sockaddr_in client_address;
				socklen_t client_addrLength = sizeof(struct sockaddr_in);
				int clientfd = accept(listen, (struct sockaddr* )&client_address, &client_addrLength);

				cout<<"client connection from: "
				    << inet_ntoa(client_address.sin_addr) << " "
					<< ntohs(client_address.sin_port)<< " " << ", clientfd = "
					<< clientfd <<endl;

				addfd(epfd, clientfd, true);


                // �������list�����û�����
				clients_list.push_back(clientfd);

				cout << "Add new clientfd = " << clientfd << "to epoll" <<endl;
				cout << "Now there are " << clients_list.size() << " clients int the chat room" << endl;

				
                // ����˷��ͻ�ӭ��Ϣ 
				cout << "welcome message" <<endl;
				char message[BUF_SIZE];
				bzero(message, BUF_SIZE);
				sprintf(message, SERVER_WELCOME, clientfd);
				int ret = send(clientfd, message, BUF_SIZE, 0);
				if (ret < 0)
				{
                    perror("send error");
                    Close();
                    exit(-1);
				}

			}
			//�����û���������Ϣ�����㲥��ʹ�����û��յ���Ϣ
			else {
				int ret = SendBroadcastMessage(sockfd);
				if (ret < 0)
				{
					perror("error");
					Close(); 
					exit(-1);
				}
			}

		}

	}
	Close(); 	
}
