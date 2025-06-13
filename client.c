// client.c
// Remote-Cmd Client: si connette, legge da stdin e riceve risposta

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SZ 4096

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    const char *port = argv[2];

    struct addrinfo hints = { .ai_family = AF_UNSPEC,
                              .ai_socktype = SOCK_STREAM };
    struct addrinfo *res;
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("getaddrinfo"); return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) { perror("socket"); return 1; }
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect"); return 1;
    }
    freeaddrinfo(res);
    printf("Connected to %s:%s. Type commands, ‘exit’ to quit.\n", host, port);

    char sendbuf[BUF_SZ], recvbuf[BUF_SZ];
    while (fgets(sendbuf, BUF_SZ, stdin)) {
        send(sock, sendbuf, strlen(sendbuf), 0);
        if (strncmp(sendbuf, "exit", 4) == 0) break;

        // ricevi fino a timeout breve o buffer pieno
        ssize_t n;
        while ((n = recv(sock, recvbuf, BUF_SZ-1, MSG_DONTWAIT)) > 0) {
            recvbuf[n] = 0;
            fputs(recvbuf, stdout);
        }
    }

    close(sock);
    return 0;
}
