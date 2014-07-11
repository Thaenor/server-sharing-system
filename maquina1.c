/********************************************
*	Trabalho de Redes de Computadores RCOMP *
*	ISEP					*
*	Francisco Santos 1111315		*
*********************************************/

/*****************Includes*******************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
/********************************************/
			/*variaveis globais*/
/*char links[81] ="a.txt;b.mp4;c.jpg"; */
int port = 60023;

/*512 e o tamanho max de um udp*/

/*************funcao cliente*****************/

/* 
A funcao cliente e responsavel por enviar 
uma mensagem UDP em broadcast (255.255.255.255)
a mensagem trata-se de uma lista com o nome dos
ficheiros existentes no servidor
*/


void *prime_client(void *x_void_ptr)
{
	char links[150] = "bananas.jpg;tese.docx;filme.mp4;";
	struct sockaddr_in me, target;
	int sock, adl, arg, i, j;
	arg = 1;

	sock = socket(AF_INET, SOCK_DGRAM,0);
	adl=sizeof(me);
	
	bzero((char*)&me,adl);
	me.sin_family = AF_INET;
	me.sin_addr.s_addr=htonl(INADDR_ANY);
	me.sin_port=htons(0);
	
	if(-1==bind(sock,(struct sockaddr *)&me,adl))
	{
		puts("Client: Porta ocupada");
		close(sock);
		exit(1);
	}
	
	/*printf("Client: socket created\n");*/
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &arg, sizeof(arg));
	for(;;)
	{
		bzero((char*)&target,adl);
		target.sin_family = AF_INET;
		/*target.sin_addr.s_addr = inet_addr("193.136.62.255");*/
		target.sin_addr.s_addr = inet_addr("255.255.255.255");
		target.sin_port=htons(port);
		
		/*printf("Client: broadcasting...\n");*/
		sendto(sock,links,150,0,(struct sockaddr *)&target,adl);
		

		/*printf("Client: sent! going to sleep\n");*/
		sleep(30);
		/*printf("Client: *wakes up* reseting cycle...\n");*/
	}
	
	close(sock);
	return NULL;
}


/*************funcao servidor****************/

/* 
A funcao servidor é responsavel por manter
e actualizar uma lista com os ficheiros dos 
restantes servidores.
*/

void *prime_server(void *x_void_ptr)
{
	struct sockaddr_in me, from;
	int sock, adl;
	char links[150]; 
	char *p, *p1;
	
	adl=sizeof(me);
	sock=socket(AF_INET,SOCK_DGRAM,0);
	
	bzero((char*)&me,adl);
	me.sin_family=AF_INET;
	me.sin_addr.s_addr=htonl(INADDR_ANY);
	me.sin_port=htons(port);

	if(-1==bind(sock,(struct sockaddr *)&me,adl))
	{
		puts("Server: Porta ocupada");
		close(sock);
		exit(1);
	}
	
	/*printf("Server: socket created %d\n",sock);

	printf("\n");
	printf("from: %d\n", from);
	printf("adl: %d\n", adl);
	printf("\n");*/

	for(;;)
	{
		recvfrom(sock,links,150,0,(struct sockaddr *)&from,&adl);
		
		/*printf("Server: received data, printing result...\n");*/
		/***/
		int i=0;
		for (i = 0; i < 150; ++i)
		{
			printf("%c", links[i]);
		}
		printf("\n");
		/***/
		
		/*p=links; p1=links1; while(*p) p++;
		p--;
		while(p>=links)
		{
			*p1=*p;p--;p1++;
		}
		*p1=0;*/
	}
	close(sock);

	return NULL;
}	


/*************funcao cgi*********************/

/* 
A funcao servidor é responsavel gerar a página
html com a lista dos ficheiros. Deve responder
a pedidos do tipo http de clientes externos
*/


void *cgi_server (void *x_void_ptr)
{
/*	char *data;
	long m,n;

	printf("%s%c%c\n","Content-Type:text/html;charset=iso-8859-1",13,10);
	printf("<title>trabalho de RCOMP 2014</title>");
	data=getenv("QUERY_STRING");
	if(data == NULL)
		printf("ERRO: dados nao passados\n");
	else if(sscanf(data, "..."))
			printf("ERRO: dados invalidos\n");
	else
		printf("lista de ficheiros...");*/

	return NULL;
}


/********************************************/
/******************  MAIN  ******************/
/********************************************/

int main(int argc, char **argv)
{
	int x = 0;
	/* this variable is our reference to the second thread */
	pthread_t server_thread;
	pthread_t client_thread;
	pthread_t cgi_thread;

/*           create threads                 */
	if(pthread_create(&server_thread, NULL, prime_server,&x)) {
		fprintf(stderr, "Error creating server side thread\n");
		return 1;
	}
 	
	if(pthread_create(&client_thread, NULL, prime_client,&x)) {
		fprintf(stderr, "Error creating client side thread\n");
		return 1;
	}

	if(pthread_create(&cgi_thread, NULL, cgi_server,&x)) {
		fprintf(stderr, "Error creating cgi server thread\n");
		return 1;
	}

	/* wait for the thread to finish */
	if(pthread_join(server_thread, NULL)) {
	fprintf(stderr, "Error joining thread\n");
 	return 2;
	}

	if(pthread_join(client_thread, NULL)) {
	fprintf(stderr, "Error joining thread\n");
 	return 2;
	}

	if(pthread_join(cgi_thread, NULL)) {
	fprintf(stderr, "Error joining thread\n");
 	return 2;
	}

	return (0);
}
