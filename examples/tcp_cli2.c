#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>   

 void main(int argc, char **argv)
 {
    struct sockaddr_in target;
    struct hostent *he;
    int sock, num;

	if(argc!=2) 
	{
		puts("falta indicar o nome do servidor");
		exit(1);
	}

	he = gethostbyname(argv[1]);
	if(!he) {printf("Falhou a resolução do nome %s\n",argv[1]);exit(1);}
	sock=socket(AF_INET,SOCK_STREAM,0);
	bzero((char *)&target,sizeof(target)); 
	target.sin_family = AF_INET; 
	target.sin_addr=*(struct in_addr *)he->h_addr_list[0];
	target.sin_port=htons(60112);

	if(connect(sock,(struct sockaddr *)&target, sizeof(target))==-1) 
	{
		puts("A ligacao falhou");
		exit(1);
	}

	do
	{
		do
		{
			scanf("%d",&num); 
			write(sock,&num,sizeof(num)); 
		}while(num); 

	read(sock,&num,sizeof(num)); 
	printf("SOMA=%d\n",num);
	}while(num);

close(sock);

}
