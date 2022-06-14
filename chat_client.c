#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //주소 변환
#include <sys/socket.h>//소켓 연결
#include <pthread.h>//쓰레드

#define BUF_SIZE 100
#define NAME_SIZE 20 //사용자 이름

void * send_msg(void * arg); //메시지 보내기
void * recv_msg(void * arg); //메시지 받기
void error_handling(char * msg); //에러 핸들링 메소드

char name[NAME_SIZE]="[DEFAULT]"; //채팅에 보여질 이름 형태. 20자 제한
char msg[BUF_SIZE]; //메시지 선언

int main(int argc,char *argv[])
{
	int sock; //소켓 선언
	struct sockaddr_in serv_addr; 
	pthread_t snd_thread,rcv_thread; //송신쓰레드,수신쓰레드
					 //메시지를 보내고 받아야 하기 때문
	void * thread_return;
	if(argc!=4) { //ip,port,이름받기
		printf("Usage : %s <IP> <port> <name>\n",argv[0]);
		exit(1);
	}

	sprintf(name,"[%s]",argv[3]); //이름 입력 받기
	sock=socket(PF_INET,SOCK_STREAM,0);//서버 소켓 연결
	
	memset(&serv_addr,0,sizeof(serv_addr)); //초기화
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		//연결
		error_handling("connect() error");
	
	//두개의 쓰레드 생성하고 각각 main은 send_msg,recv_msg로
	pthread_create(&snd_thread,NULL,send_msg,(void*)&sock); //메시지 보내고
	pthread_create(&rcv_thread,NULL,recv_msg,(void*)&sock);//받음

	pthread_join(snd_thread,&thread_return); //send 쓰레드 소멸
	pthread_join(rcv_thread,&thread_return); //recv 쓰레드 소멸
	close(sock); //클라이언트 연결 종료
	return 0;
}

void * send_msg(void * arg) //send thread main
{
	int sock=*((int*)arg); //int형으로 전환
	char name_msg[NAME_SIZE+BUF_SIZE];//사용자이름이랑 메시지 합쳐서 보냄
	while(1)
	{
		fgets(msg,BUF_SIZE,stdin); //입력
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))//q입력시 종료
		{
			close(sock); //종료
			exit(0);
		}
		sprintf(name_msg,"%s %s",name,msg); //출력
		write(sock,name_msg,strlen(name_msg));
	}
	return NULL;
}
void * recv_msg(void * arg) //read thread main
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	int str_len;
	while(1)
	{
		str_len=read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);
		if(str_len==-1) //서버 소켓과 연결이 끊어졌다면 -> send_msg에서 close(sock)실행되고, 서버가 자신이 가신 클라이언트 소켓을 close한 후 read해서 끊어진 경우
			return (void*)-1; //pthread_join 실행 준비
		name_msg[str_len]=0; //버퍼 맨 마지막 null로
		fputs(name_msg,stdout); //받은 메시지 출력
	}
	return NULL;
}
void error_handling(char *msg) //에러 핸들링
{
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}
