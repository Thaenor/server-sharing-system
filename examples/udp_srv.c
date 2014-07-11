#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void main(int argc, char **argv)
{
	struct sockaddr_in me, from;
	int sock, adl;
	char linha[81],linha1[81];
	char *p, *p1;
	
	adl=sizeof(me);
	sock=socket(AF_INET,SOCK_DGRAM,0);
	
	bzero((char*)&me,adl);
	me.sin_family=AF_INET;
	me.sin_addr.s_addr=htonl(INADDR_ANY);
	me.sin_port=htons(60011);
	
	if(-1==bind(sock,(struct sockaddr *)&me,adl))
	{
		puts("Porta ocupada");
		close(sock);
		exit(1);
	}
	
	for(;;)
	{
		recvfrom(sock,linha,81,0,(struct sockaddr *)&from,&adl);
		printf("Recebi %s\n",linha);
		
		p=linha; p1=linha1; while(*p) p++;
		p--;
		while(p>=linha)
			{
			*p1=*p;p--;p1++;
			}
		*p1=0;

		sendto(sock,linha1,81,0,(struct sockaddr *)&from,adl);
	}
	close(sock);
}	
