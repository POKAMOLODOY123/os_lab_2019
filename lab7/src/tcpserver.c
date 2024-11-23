#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const size_t kSize = sizeof(struct sockaddr_in);
    int lfd, cfd;
    int nread;
    char buf[1024]; // Убрали BUFSIZE, размер буфера теперь через аргумент
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <port> <bufsize>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int bufsize = atoi(argv[2]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr,"Неверный номер порта. Допустимый диапазон 1-65535.\n");
        exit(1);
    }

    if (bufsize <= 0 || bufsize > 1024) { // проверка на валидность буфера
        fprintf(stderr,"Неверный размер буфера.  Допустимый диапазон 1-1024.\n");
        exit(1);
    }
    // теперь bufsize определяет размер буфера.
    // Создаем буфер нужного размера.
    char buf2[bufsize];

    if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(lfd, (struct sockaddr *)&servaddr, kSize) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(lfd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    while (1) {
        unsigned int clilen = kSize;

        if ((cfd = accept(lfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
            perror("accept");
            exit(1);
        }
        printf("connection established\n");

        while ((nread = read(cfd, buf2, bufsize)) > 0) { // используем buf2 и bufsize
            write(1, buf2, nread);
        }

        if (nread == -1) {
            perror("read");
            exit(1);
        }
        close(cfd);
    }
}

