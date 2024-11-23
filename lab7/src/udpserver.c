#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, n;
    char mesg[1024]; // Убрали BUFSIZE, размер буфера теперь через аргумент
    char ipadr[16];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bufsize> <port>\n", argv[0]);
        exit(1);
    }
    int bufsize = atoi(argv[1]);
    int port = atoi(argv[2]);


    if (bufsize <= 0 || bufsize > 1024) { // проверка на валидность буфера
        fprintf(stderr,"Неверный размер буфера.  Допустимый диапазон 1-1024.\n");
        exit(1);
    }

    if (port <= 0 || port > 65535) {
        fprintf(stderr,"Неверный номер порта. Допустимый диапазон 1-65535.\n");
        exit(1);
    }

    // теперь bufsize определяет размер буфера. Создаем буфер нужного размера.
    char mesg2[bufsize];


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind problem");
        exit(1);
    }
    printf("SERVER starts...\n");

    while (1) {
        unsigned int len = sizeof(cliaddr);

        if ((n = recvfrom(sockfd, mesg2, bufsize, 0, (struct sockaddr *)&cliaddr, &len)) < 0) { // используем mesg2 и bufsize
            perror("recvfrom");
            exit(1);
        }
        mesg2[n] = 0;

        printf("REQUEST %s      FROM %s : %d\n", mesg2,
               inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
               ntohs(cliaddr.sin_port));

        if (sendto(sockfd, mesg2, n, 0, (struct sockaddr *)&cliaddr, len) < 0) { // используем mesg2
            perror("sendto");
            exit(1);
        }
    }
}

