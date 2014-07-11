#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void main(int argc, char **argv)
{
	struct sockaddr_in me, target;
	int sock, adl;
	char linha[81];
	
	if(argc != 2){puts("falha a indicar endereço de IP do servidor"); exit(1);}
	sock = socket(AF_INET, SOCK_DGRAM,0);
	adl=sizeof(me);
	
	bzero((char*)&me,adl);
	me.sin_family = AF_INET;
	me.sin_addr.s_addr=htonl(INADDR_ANY);
	me.sin_port=htons(0);
	
	bind(sock,(struct sockaddr *)&me,adl);
	
	bzero((char*)&target,adl);
	target.sin_family = AF_INET;
	target.sin_addr.s_addr = inet_addr(argv[1]);
	target.sin_port=htons(60011);
	
	do
		{
		gets(linha);
		sendto(sock,linha,81,0,(struct sockaddr*)&target,adl);
		recvfrom(sock,linha,81,0,(struct sockaddr*)&target,&adl);
		puts(linha);
		}
	while(strcmp(linha,"rias"));
	close(sock);
}
