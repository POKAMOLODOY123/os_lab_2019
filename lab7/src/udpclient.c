#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, n;
    char sendline[1024]; // Убрали BUFSIZE, размер буфера теперь через аргумент
    char recvline[1024 + 1];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if (argc < 4) {
        fprintf(stderr, "usage: client <IPaddress of server> <port> <bufsize>\n");
        exit(1);
    }
    int port = atoi(argv[2]);
    int bufsize = atoi(argv[3]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr,"Неверный номер порта. Допустимый диапазон 1-65535.\n");
        exit(1);
    }

    if (bufsize <= 0 || bufsize > 1024) { // проверка на валидность буфера
        fprintf(stderr,"Неверный размер буфера.  Допустимый диапазон 1-1024.\n");
        exit(1);
    }


    // теперь bufsize определяет размер буфера. Создаем буферы нужного размера.
    char sendline2[bufsize];
    char recvline2[bufsize + 1];


    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
        perror("inet_pton problem");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        exit(1);
    }

    write(1, "Enter string\n", 13);

    while ((n = read(0, sendline2, bufsize)) > 0) {  // Используем sendline2 и bufsize
        if (sendto(sockfd, sendline2, n, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
            perror("sendto problem");
            exit(1);
        }

        if (recvfrom(sockfd, recvline2, bufsize, 0, NULL, NULL) == -1) { // Используем recvline2 и bufsize
            perror("recvfrom problem");
            exit(1);
        }

        printf("REPLY FROM SERVER= %s\n", recvline2);
    }
    close(sockfd);
}

