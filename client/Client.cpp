#include <iostream> 
#include "Client.h"

using namespace std;

Client::Client(){
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // ��ʼ��socket
	sock = 0;

    // epoll fd
	epfd = 0;

    // ��ʼ�����̺�
	pid = 0;
    // �ͻ���״̬
	isClientwork = true;
}

// ���ӷ�����
void Client::Connect() {
	cout << "Connect Server: "<< SERVER_IP<< " : " << SERVER_PORT << endl;

    // ����socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("sock error");
		exit(-1);
	}

    // ���ӷ����
	if (connect(sock, (struct sockaddr *)&serverAddr), sizeof(serverAddr), 0) < 0)
	{
		perror("connect error");
		exit(-1);
	}
     // �����ܵ�������fd[0]���ڸ����̶���fd[1]�����ӽ���д

	 if (pipe(pipe_fd) < 0)
	 {
        perror("pipe error");
        exit(-1);
	 }


    // ����epoll
	epfd = epoll_create(EPOLL_SIZE);
	if (epfd < 0)
	{
        perror("epfd error");
        exit(-1);
	}

	addfd(epfd, sock, true);
	addfd(epfd, pipe_fd[0],true);

}


// �Ͽ����ӣ������ر��ļ�������
void Client::Close() {
	
	if (pid)
	{
		
       //�رո����̵Ĺܵ���sock
		close(pipe_fd[0]);
		close(sock);
	}
	else {

        //�ر��ӽ��̵Ĺܵ�
		close(pipe_fd[1]);
	}
}


void Client::Start(){

 
    // epoll �¼�����	
	struct epoll_event events[2];

	// ���ӷ�����
	Connect();


    // �����ӽ���
	pid = fork();

	if (pid < 0)
	{
		perror("fork error");
		close(sock);
		exit(-1);
	}
	else if(pid == 0) {
        // �����ӽ���ִ������
        //�ӽ��̸���д��ܵ�������ȹرն���
		close(pipe_fd[0]);

        // ����exit�����˳�������
        cout << "Please input 'exit' to exit the chat room" << endl;
        cout<<"\\ + ClientID to private chat "<<endl;
		

        // ����ͻ������������򲻶϶�ȡ���뷢�͸������
		while(isClientwork){
			memset(msg.content, 0, sizeof(msg.content));
			fgets(msg.content, BUF_SIZE, stdin);
			
			if(strncasecmp(msg.content, EXIT, strlen(EXIT)) == 0){
				isClientwork = 0;
			}

            // �ӽ��̽���Ϣд��ܵ�
			else {
				memset(send_buf, 0, BUF_SIZE);
				memcpy(send_buf, &msg, sizeof(msg));
				if (write(pipe_fd[1], send_buf, sizeof(send_buf)) < 0)
				{

                    perror("fork error");
                    exit(-1);
				}
			}
		}

	}
	else {

        //pid > 0 ������
        //�����̸�����ܵ����ݣ�����ȹر�д��		
		close(pipe_fd[1]);

		while (isClientwork) 
		{
			int epoll_events_count = epoll_wait(epfd, events, 2, -1);
			for (int i = 0; i < epoll_events_count; i++)
			{
				memset(recv_buf, 0, sizeof(recv_buf));

				if (events[i].data.fd == sock){
					int ret = recv(sock, recv_buf, BUF_SIZE, 0);

					memset(&msg, 0, sizeof(msg));
					memcpy(&msg, recv_buf, sizeof(recv_buf));

					if (ret == 0){
						cout << "Server closed connection: " << sock << endl;
						close(sock);
						isClientwork = 0;
					}
					else {
						cout << msg.content<<endl;
					}
				}
				else {
					
                    //�����̴ӹܵ��ж�ȡ����
					int ret = read(events[i].data.fd, recv_buf, BUF_SIZE);

					if (ret == 0){

						isClientwork = 0;
					}
					else {

						send(send, recv_buf, sizeof(recv_buf), 0);
					}
				}

			}
		}
	}
	Close();

}