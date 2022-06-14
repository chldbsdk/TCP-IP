#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //주소변환
#include <sys/socket.h> //소켓 연결
#include <netinet/in.h> //IPV4 전용 기능
#include <pthread.h> //쓰레드 사용

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg); //클라이언트로부터 받은 메시지 처리 함수
void send_msg(char * msg,int len);//클라이언트한테 메시지 받는 함수
void error_handling(char * msg); //에러

int clnt_cnt=0; //클라이언트의 소켓 관리를 위한 변수. 클라이언트 수
int clnt_socks[MAX_CLNT]; //클라이언트의 소켓 관리를 위한 배열.최대 클라이언트 소켓
pthread_mutex_t mutx; //쓰레드 동시접근 해결

int main(int argc,char *argv[])
{
	int serv_sock,clnt_sock; //서버소켓,클라이언트 소켓 변수 선언
	struct sockaddr_in serv_adr,clnt_adr; //서버,클라이언트 주소 구조체선언
	int clnt_adr_sz; //클라이언트 주소 크기
	pthread_t t_id; //쓰레드 아이디
	if(argc!=2) { //포트번호
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx,NULL); //mutex 초기화.생성
	serv_sock=socket(PF_INET,SOCK_STREAM,0);//IPV4 연결지향형 소켓 생성
	memset(&serv_adr,0,sizeof(serv_adr)); //소켓 정보 초기화
	serv_adr.sin_family=AF_INET; //IPV4 사용
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //IP를 반환 후 입력
	serv_adr.sin_port=htons(atoi(argv[1]));//포트입력

	if(bind(serv_sock,(struct sockaddr*) &serv_adr,sizeof(serv_adr))==1)
		//바인딩으로 IP,포트번호 할당
		error_handling("bind() error");
	if(listen(serv_sock,5)==-1) //소켓 연결요청가능 상태로 설정
		error_handling("listen() error");

	while(1) //값 받아오기
	{
		clnt_adr_sz=sizeof(clnt_adr);//크기할당
		clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz); //클라이언트 접속 대기
		pthread_create(&t_id,NULL,handle_clnt,(void*)&clnt_sock); //쓰레드 생성, 함수 실행
		pthread_detach(t_id); //pthread_detach 함수호출을 통해서 종료된 쓰레드가 메모리에서 완전히 소멸되도록 한다.
		printf("Connected client IP: %s \n",inet_ntoa(clnt_adr.sin_addr));//접속 클라이언트 IP 출력
	}
	close(serv_sock);//서버 소켓 종료
	return 0;
}

void * handle_clnt(void * arg) //클라이언트가 보낸 메시지 핸들링
{
	int clnt_sock=*((int*)arg);//입력 받은 소켓 대입
	int str_len=0,i;//문자열 길이 선언
	char msg[BUF_SIZE]; //메시지 크기 100으로 설정
	while((str_len=read(clnt_sock,msg,sizeof(msg)))!=0)
		//클라이언트가 보낸 메시지 수신
		send_msg(msg,str_len); //메시지 읽는 함수 실행

	pthread_mutex_lock(&mutx); //먼저 생성된 쓰레드 보호
	for(i=0;i<clnt_cnt;i++) //removew disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx); //쓰레드 보호 해제
	close(clnt_sock);//클라이언트 종료
	return NULL;
}
void send_msg(char * msg,int len) //send to all. 연결된 모든 클라이언트에게 메시지 전송
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_cnt;i++) //클라이언트 소켓에 메시지 출력
		write(clnt_socks[i],msg,len);
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg) //에러 함수
{
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}
