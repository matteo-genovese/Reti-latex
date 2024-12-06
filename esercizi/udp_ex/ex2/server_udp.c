#include <stdio.h>
#include <stdlib.h>     /* exit() */
#include <string.h>    /* bzero(), bcopy() */
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>       /* time_t e ctime() */
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */
#include <arpa/inet.h> 

#define MAXLINE 4096

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd;
    int portno, clilen;
    int bytesread, counter;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[MAXLINE], readLine[MAXLINE];

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
    	error("ERROR opening socket");
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
 		error("ERROR on binding");
	}

	clilen = sizeof(cli_addr);
	counter = 0;
	for (;;) {
 		bzero(buffer, MAXLINE);
 		bytesread = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		printf("Server received packet from IP: %s, Port: %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
 		printf("check\n");
 		if (!strcmp(buffer, "T\n")) {
          counter++;
          time_t t = time(NULL);
          char* timestr = ctime(&t);
          sendto(sockfd, timestr, strlen(timestr)+1, 0, (struct sockaddr *)&cli_addr, clilen);
        } else if (!strcmp(buffer, "N\n")) {
          char cnt[5];
          sprintf(cnt, "%d\n", counter);
          sendto(sockfd, cnt, strlen(cnt)+1, 0, (struct sockaddr *)&cli_addr, clilen);
        } else {
          sendto(sockfd, "ricevuto\n", 11, 0, (struct sockaddr *)&cli_addr, clilen);
        }
 	}
}
