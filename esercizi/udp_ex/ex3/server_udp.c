#include <stdio.h>
#include <stdlib.h>     /* exit() */
#include <string.h>    /* bzero(), bcopy() */
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>       /* time_t e ctime() */
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */
#include <arpa/inet.h> 
#include <ctype.h>

#define MAXLINE 4096

void error(char *msg) {
    perror(msg);
    exit(1);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	len_s1;
	size_t	len_s2;
	size_t	i;
	char	*str;

	if (!s1 || !s2)
		return (NULL);
	len_s1 = strlen(s1);
	len_s2 = strlen(s2);
	str = (char *)malloc((len_s1 + len_s2 + 1) * sizeof(char));
	if (!str)
		return (NULL);
	i = -1;
	while (++i < len_s1 && s1[i])
		str[i] = s1[i];
	i = -1;
	while (++i < len_s2 && s2[i])
		str[len_s1 + i] = s2[i];
	str[len_s1 + len_s2] = '\0';
	return (str);
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
		for (int i = 0; i < bytesread; i++) {
			buffer[i] = toupper(buffer[i]);
		}
		char *str = ft_strjoin("Buffer but in uppercase: ", buffer);
		sendto(sockfd, str, strlen(str), 0, (struct sockaddr *)&cli_addr, clilen);
		free(str);
	}
}

