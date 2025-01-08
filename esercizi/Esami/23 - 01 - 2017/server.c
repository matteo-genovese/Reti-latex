#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>       /* funzioni time() e ctime() */
#include <unistd.h>
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */
#include <netinet/in.h>

/* 
Completare il codice fornito per realizzare il server di un’applicazione basata sul protocollo TCP.
Il server deve essere avviato da riga di comando, accettando come parametri il numero di porta in
cui deve stare in ascolto e il nome simbolico del server (Server-name). Il client invia al Server
messaggi del tipo ”Hello! My name is client-name.”. Nel caso in cui il messaggio del client sia
formattato correttamente, il server risponde fornendo un messaggio del tipo ”Hello client-name!
My name is Server-name.” Se il messaggio proveniente dal client non e’ formattato correttamente,
il server risponde con il messaggio ”Wrong Message”.
*/

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd;
	int portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n, counter;
	pid_t pid;

	counter = 0;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}
 
	listen(sockfd, 1);

	while (1) {
		char* str;
		char* nstr;
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
		if (newsockfd < 0) {
			error("Nicols");
		}

		pid=fork();

		if (pid == 0) {
			close(sockfd);

			bzero(buffer, 256);
			n = read(newsockfd, buffer, 255);
			if (n < 0) {
				error("Nicols lettura");
			}
			
			str = strdup(buffer + 17);

			if (!strncmp(buffer, "Hello! My name is ", 18) && !strchr(buffer + strlen(buffer) - 1, '.')) {
				char client_name[256];
				strncpy(client_name, buffer + 18, strlen(buffer)); // Copia il nome senza il punto finale

				// Prepara il messaggio di risposta
				char response[512];
				snprintf(response, sizeof(response), "\nHello %s! My name is %s. \n", client_name, argv[2]);
				n = write(newsockfd, response, strlen(response));
				if (n < 0) {
					error("Potillozzo di dio");
				}
			} else {
				n = write(newsockfd, "Wrong Message\n", strlen("Wrong Message\n"));
			}
		
			close(newsockfd);
			return 0;
		}

		close(newsockfd);
	} 
	return 0;
}
