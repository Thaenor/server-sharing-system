#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void main(void)
{
	struct sockaddr_in me, from;
	int sock, adl;
	char linha[81];
	
	adl = sizeof(me);
	sock = socket(AF_INET,SOCK_DGRAM,0);
	
	bzero((char *)&me,adl);
	me.sin_family = AF_INET;
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	me.sin_port=htons(7013);

	if(-1==bind(sock,(struct sockaddr *)&me, adl))
	{
		puts("Porta Ocupada");
		close(sock);
		exit(1);
	}
	
	do
	{
		recvfrom(sock,linha,81,0,(struct sockaddr *)&from,&adl);
		puts(linha);
	}
	while(strcmp(linha,"exit"));

	close(sock);
}
