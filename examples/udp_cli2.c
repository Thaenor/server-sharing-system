#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

void main()
{
	struct sockaddr_in me, target;
	int sock, adl, um;
	char linha[81];
	
	ulong ip1 = 0; /*unsigned long*/

	sock = socket(AF_INET, SOCK_DGRAM,0);
	adl=sizeof(me);
	um=1;
	
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &um, sizeof(um));
	
	bzero((char*)&me,adl);
	me.sin_family = AF_INET;
	me.sin_addr.s_addr=htonl(INADDR_ANY);
	me.sin_port=htons(0);
	
	bind(sock,(struct sockaddr *)&me,adl);
	
	bzero((char*)&target,adl);
	target.sin_family = AF_INET;
	target.sin_addr.s_addr = inet_addr("255.255.255.255");
	target.sin_port=htons(7004);
	
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
