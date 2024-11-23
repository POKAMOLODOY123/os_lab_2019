#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd;
    int nread;
    char buf[1024]; // Убрали BUFSIZE, размер буфера будем задавать через аргумент
    struct sockaddr_in servaddr;


    if (argc < 4) {
        printf("Usage: %s <IP address> <port> <bufsize>\n", argv[0]);
        exit(1);
    }

    int bufsize = atoi(argv[3]); // размер буфера из аргументов командной строки

    if (bufsize <= 0 || bufsize > 1024) { // проверка на валидность буфера
        fprintf(stderr,"Неверный размер буфера.  Допустимый диапазон 1-1024.\n");
        exit(1);
    }
    char buf2[bufsize]; // теперь bufsize определяет размер буфера

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creating");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr)); // Используем sizeof для большей надежности
    servaddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("bad address");
        exit(1);
    }

    servaddr.sin_port = htons(atoi(argv[2]));

    if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { // Используем sizeof для большей надежности
        perror("connect");
        exit(1);
    }

    write(1, "Input message to send\n", 22);
    while ((nread = read(0, buf2, bufsize)) > 0) { // используем buf2 и bufsize
        if (write(fd, buf2, nread) < 0) {
            perror("write");
            exit(1);
        }
    }

    close(fd);
    exit(0);
}

