#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>       /* funzioni time() e ctime() */
#include <unistd.h>
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */
#include <netinet/in.h>

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

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     listen(sockfd,5);

     do {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                    (struct sockaddr *) &cli_addr,
                    (socklen_t *)&clilen);

        if (newsockfd < 0) {
              error("ERROR on accept");
        }

        pid=fork();

        if (pid == 0) {

          close(sockfd);

          bzero(buffer,256);
          n = read(newsockfd,buffer,255);
          if (n < 0) {
            error("ERROR reading from socket");
          }

          if(!strcmp(buffer, "T\n")) {
            counter++;

            time_t t = time(NULL);

            char* timestr = ctime(&t);
            n = write(newsockfd, timestr, strlen(timestr)+1);
            if (n < 0) {
              error("ERROR writing to socket");
            }
          }

		  else if(!strcmp(buffer, "N\n")) {
              char cnt[5];
            sprintf(cnt, "%d", counter);
            n = write(newsockfd, cnt, strlen(cnt));
            if (n < 0) {
              error("ERROR writing to socket");
            }
          }

		  else {
            // delay
            printf("Please press a key...");
            char c;
            scanf("%c", &c);
            n = write(newsockfd, "Message received", 20);
            if (n < 0) {
              error("ERROR writing to socket");
            }
          }

          close(newsockfd);
          return 0;
        }

        close(newsockfd);


     } while (1);
     return 0;
}
