#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

void main(void)
{
	struct sockaddr_in me, from;
	int newSock, sock, adl;
	int num, soma; 

	adl=sizeof(me);
	sock=socket(AF_INET,SOCK_STREAM,0); 

	bzero((char *)&me, adl);
	me.sin_family=AF_INET;
	me.sin_addr.s_addr=htonl(INADDR_ANY);
	me.sin_port=htons(60112); 

	if(-1==bind(sock,(struct sockaddr *)&me, adl))
		{ 
			puts("Porta ocupada"); 
			close(sock); 
			exit(1);
		}
	
	 listen(sock,SOMAXCONN); 
	 for (;;)
	 {
	 	newSock=accept(sock,(struct sockaddr *)&from,&adl);
		
		if(!fork()) 
		{
			close(sock);
			do
			{
				soma = 0;
				do
				{
					 read(newSock,&num,sizeof(num)); 
					 soma = soma + num;
				}
				while(num);
				write(newSock,&soma,sizeof(soma));
			}
			while(soma);
			close(newSock);
			exit(0);
		}
			close(newSock);
	}	
	
	close(sock);
}