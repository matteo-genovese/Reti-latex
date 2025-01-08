#include <stdio.h>
#include <stdlib.h>  /* exit() */
#include <strings.h> /* bzero(), bcopy() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg) {
    perror(msg);
    exit(1);
}

/* Completare il codice fornito per realizzare il client di un’applicazione basata sul protocollo TCP,
che consenta di ottenere l’orario dal server solo se si e’ a conoscenza di una parola segreta. Il client,
al momento dell’avvio deve accettare da riga di comando l’indirizzo e la porta a cui contattare il
server. Deve poi chiedere all’utente di inserire la parola segreta ed inviarla al server. Nel caso in
cui la parola segreta sia corretta, ricevera’ dal server l’ora corrente, che andra’ visualizzata sullo
standard output prima di terminare l’esecuzione. Nel caso in cui la parola segreta sia errata, il
client informera’ l’utente dell’errore e richiedera’ la parola segreta. Dopo tre tentativi falliti il client
terminera’ la sua esecuzione. */

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256], pwd[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s servername port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR connect");
	}

	printf("Enter the secret word for the time info: \n");
	for (size_t i = 0; i < 3; ++i) {
		bzero(pwd, 256);
		fgets(pwd, 255, stdin);
		n = write(sockfd, pwd, strlen(pwd)); 
		if (n < 0) {
			error("ERROR on writing");
		}
		
		bzero(buffer, 256);
		n = read(sockfd, buffer, 255);
		if (n < 0) {
			error("ERROR on reading");
		}

		if (!strcmp(buffer, "POTILLOZZO")) {
			bzero(buffer, 256);
			n = read(sockfd, buffer, 255);
			if (n < 0) {
				error("ERROR on reading the time from the server");
			}
			printf("%s", buffer);
			i = 3;
			printf("QUIT.");
			close(sockfd); 
		} else {
			printf("The secret word is wrong, retry.\n");
		}
	}
	close(sockfd);
	return 0;
}
