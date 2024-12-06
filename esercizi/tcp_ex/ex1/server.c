#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  /* inet_ntoa() */

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main (int argc, char** argv) {
    int sockfd, newsocketfd, portno, clilen;
    int n, count;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];
    pid_t pid;

    int f;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(1);
    }
    
    listen(sockfd, f);

    do {
        clilen = sizeof(cli_addr);
        newsocketfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
        if (newsocketfd < 0) {
            perror("Error on accept");
            exit(1);
        }
        
        // Stampa dei parametri di rete del client
        printf("Connection from client IP: %s, Port: %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        
        pid = fork();
        if (pid == 0) {
            close(sockfd);
            bzero(buffer, 256);
            n = read(newsocketfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                exit(1);
            }
            printf("Here is the message: %s\n", buffer);
            n = write(newsocketfd, "I got your message", 18);
            if (n < 0) {
                perror("Error writing to socket");
                exit(1);
            }
            exit(0);
        } else {
            close(newsocketfd);
        }
    } while (1);
}
