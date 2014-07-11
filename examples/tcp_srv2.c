#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

void main(int argc, char **argv)
{
	struct sockaddr_in me, from;
	int newSock, sock, adl;
	int num, soma; 

	if(argc < 2) 
	{
		puts("falta de argumentos\n");
		exit(1);
	}


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


			int i;
			for (i = 0; i < argc; ++i)
			{
				if(!strcmp(argv[i]),inet_ntoa(from.sin_addr))
					break;
				if (i==argc)
				{
					printf("cliente recusado:%s\n", inet_ntoa(from.sin_addr) );
					close(newSock);
					exit(0);
				}				
			}

		printf("cliente aceite:%s\n",inet_ntoa(from.sin_addr));

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