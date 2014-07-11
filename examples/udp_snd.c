#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void main(void)
{
	struct sockaddr_in me, target;
	int sock;
	char linha[81];


	sock = socket(AF_INET,SOCK_DGRAM,0);

	bzero((char *)&me,sizeof(me));
	me.sin_family = AF_INET;
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	me.sin_port=htons(0);

	bind(sock,(struct sockaddr *)&me,sizeof(me));

	bzero((char *)&target,sizeof(target));
	target.sin_family = AF_INET;
	target.sin_addr.s_addr = inet_addr("193.136.62.38");
	target.sin_port=htons(7013);

	do
	{
		gets(linha);
		sendto(sock,linha,81,0,(struct sockaddr *)&target,sizeof(target));
	}
	while(strcmp(linha,"exit"));
	
	close(sock);
}
