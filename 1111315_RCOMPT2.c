/********************************************
*	Trabalho de Redes de Computadores RCOMP *
*	ISEP					*
*	Francisco Santos 1111315		*
*********************************************/

/*****************Includes*******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 30473 /*HTTP listening port*/
#define CONNMAX 1000
#define BYTES 1024
#define SERVER "webserver/1.0"
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

void send_error(FILE *f, int status, char *title, char *extra, char *text);
void send_headers(FILE *f, int status, char *title, char *extra, char *mime, int length, time_t date);
int process(FILE *f);
void send_file(FILE *f, char *path, struct stat *statbuf);
char *get_mime_type(char *name);
/********************************************/
			/*variaveis globais*/
/*char links[81] ="a.txt;b.mp4;c.jpg"; */
/*este e o porto usado para comunicacao entre servidores*/
int port = 60023;
char links[150];
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
	strcpy(links, "musica.mp3 foto.jpg texto.txt\n");
	struct sockaddr_in me, target;
	int sock, adl, arg;
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

	struct sockaddr_in me, from;
	int newSock, sock, adl;	

	adl=sizeof(me); 
	sock=socket(AF_INET,SOCK_STREAM,0);

	bzero((char *)&me, adl);
	me.sin_family=AF_INET; 
	me.sin_addr.s_addr=htonl(INADDR_ANY); 
	me.sin_port=htons(PORT);

	if(-1==bind(sock,(struct sockaddr *)&me, adl))
	{ 
		puts("Porta ocupada"); 
		close(sock); 
		exit(1);
	}

	listen(sock,SOMAXCONN);
	printf("HTTP server listening on port %d\n", PORT);

	while(1)
	{
		int s;
    	FILE *f;

    	s = accept(sock, NULL, NULL);
    	if (s < 0) break;

    	f = fdopen(s, "a+");
    	process(f);
    	fclose(f);
		
	}
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


/***/

void send_error(FILE *f, int status, char *title, char *extra, char *text) {
  send_headers(f, status, title, extra, "text/html", -1, -1);
  fprintf(f, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\r\n", status, title);
  fprintf(f, "<BODY><H4>%d %s</H4>\r\n", status, title);
  fprintf(f, "%s\r\n", text);
  fprintf(f, "</BODY></HTML>\r\n");
}

/****/

void send_headers(FILE *f, int status, char *title, char *extra, char *mime, 
                  int length, time_t date) {
  time_t now;
  char timebuf[128];

  fprintf(f, "%s %d %s\r\n", PROTOCOL, status, title);
  fprintf(f, "Server: %s\r\n", SERVER);
  now = time(NULL);
  strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
  fprintf(f, "Date: %s\r\n", timebuf);
  if (extra) fprintf(f, "%s\r\n", extra);
  if (mime) fprintf(f, "Content-Type: %s\r\n", mime);
  if (length >= 0) fprintf(f, "Content-Length: %d\r\n", length);
  if (date != -1) {
    strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&date));
    fprintf(f, "Last-Modified: %s\r\n", timebuf);
  }
  fprintf(f, "Connection: close\r\n");
  fprintf(f, "\r\n");
}

/***/







int process(FILE *f) {
  char buf[4096];
  char *method;
  char *path;
  char *protocol;
  struct stat statbuf;
  char pathbuf[4096];
  int len;

  if (!fgets(buf, sizeof(buf), f)) return -1;
  printf("URL: %s", buf);

  method = strtok(buf, " ");
  path = strtok(NULL, " ");
  protocol = strtok(NULL, "\r");
  if (!method || !path || !protocol) return -1;

  fseek(f, 0, SEEK_CUR); // Force change of stream direction

  if (strcasecmp(method, "GET") != 0) {
    send_error(f, 501, "Not supported", NULL, "Method is not supported.");
  } else if (stat(path, &statbuf) < 0) {
    send_error(f, 404, "Not Found", NULL, "File not found.");
  } else if (S_ISDIR(statbuf.st_mode)) {
    len = strlen(path);
    if (len == 0 || path[len - 1] != '/') {
      snprintf(pathbuf, sizeof(pathbuf), "Location: %s/", path);
      send_error(f, 302, "Found", pathbuf, "Directories must end with a slash.");
    } else {
      snprintf(pathbuf, sizeof(pathbuf), "%sindex.html", path);
      if (stat(pathbuf, &statbuf) >= 0) {
        //send_file(f, pathbuf, &statbuf);
      } else {
        DIR *dir;
        struct dirent *de;

        send_headers(f, 200, "OK", NULL, "text/html", -1, statbuf.st_mtime);
        fprintf(f, "<HTML><HEAD><TITLE>Index of %s</TITLE></HEAD>\r\n<BODY>", path);
        fprintf(f, "<H4>Index of %s</H4>\r\n<PRE>\n", path);
        fprintf(f, "Name                             Last Modified              Size\r\n");
        fprintf(f, "<HR>\r\n");
        if (len > 1) fprintf(f, "<A HREF=\"..\">..</A>\r\n");

        dir = opendir(path);

        int cont;
          for (cont = 0; cont < 150; ++cont)
          {
          	fprintf(f, "%c", links[cont]);
          }
        fprintf(f, "<script> function reloadPage() {setTimeout(refresh, 10000);}</script> ");

        while ((de = readdir(dir)) != NULL) {
          char timebuf[32];
          struct tm *tm;

          strcpy(pathbuf, path);
          strcat(pathbuf, de->d_name);

          stat(pathbuf, &statbuf);
          tm = gmtime(&statbuf.st_mtime);
          strftime(timebuf, sizeof(timebuf), "%d-%b-%Y %H:%M:%S", tm);
          
          fprintf(f, "<script> function reloadPage() {setTimeout(refresh, 10000);}</script> ");
          //fprintf(f, "<input type=\"button\" value=\"Reload page\" onclick=\"reloadPage()\">");

          fprintf(f, "<A HREF=\"%s%s\">", de->d_name, S_ISDIR(statbuf.st_mode) ? "/" : "");
          fprintf(f, "%s%s", de->d_name, S_ISDIR(statbuf.st_mode) ? "/</A>" : "</A> ");
          if (strlen(de->d_name) < 32) fprintf(f, "%*s", 32 - strlen(de->d_name), "");
          if (S_ISDIR(statbuf.st_mode)) {
            fprintf(f, "%s\r\n", timebuf);
          } else {
            fprintf(f, "%s %10d\r\n", timebuf, statbuf.st_size);
          }
        }
        closedir(dir);

        fprintf(f, "</PRE>\r\n<HR>\r\n<ADDRESS>%s</ADDRESS>\r\n</BODY></HTML>\r\n", SERVER);
      }
    }
  } else {
    //send_file(f, path, &statbuf);
  }

  return 0;
}


























void send_file(FILE *f, char *path, struct stat *statbuf) {
  char data[4096];
  int n;

  FILE *file = fopen(path, "r");
  if (!file) {
    send_error(f, 403, "Forbidden", NULL, "Access denied.");
  } else {
    int length = S_ISREG(statbuf->st_mode) ? statbuf->st_size : -1;
    send_headers(f, 200, "OK", NULL, get_mime_type(path), length, statbuf->st_mtime);

    while ((n = fread(data, 1, sizeof(data), file)) > 0) fwrite(data, 1, n, f);
    fclose(file);
  }
}


char *get_mime_type(char *name) {
  char *ext = strrchr(name, '.');
  if (!ext) return NULL;
  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
  if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
  if (strcmp(ext, ".gif") == 0) return "image/gif";
  if (strcmp(ext, ".png") == 0) return "image/png";
  if (strcmp(ext, ".css") == 0) return "text/css";
  if (strcmp(ext, ".au") == 0) return "audio/basic";
  if (strcmp(ext, ".wav") == 0) return "audio/wav";
  if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
  if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg";
  if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
  return NULL;
}