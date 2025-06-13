// client.c v2
// Remote-Cmd Client con help, host/port configurabili e dry-run

#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SZ 4096

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s -h host -p port [-d]\n"
        "  -h host    server hostname o IP\n"
        "  -p port    server TCP port\n"
        "  -d         dry-run (non invia nulla)\n"
        "  -?         this help\n",
        prog);
    exit(1);
}

int main(int argc, char **argv) {
    char *host = NULL, *port = NULL;
    int dry_run = 0, opt;

    while ((opt = getopt(argc, argv, "h:p:d?")) != -1) {
        switch (opt) {
        case 'h': host = optarg;         break;
        case 'p': port = optarg;         break;
        case 'd': dry_run = 1;           break;
        case '?':
        default:  usage(argv[0]);
        }
    }
    if (!host || !port) usage(argv[0]);

    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM
    }, *res;
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    if (!dry_run && connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        return 1;
    }
    freeaddrinfo(res);

    printf("** %s %s (dry_run=%d) **\n", host, port, dry_run);
    char sendbuf[BUF_SZ], recvbuf[BUF_SZ];
    while (fgets(sendbuf, sizeof(sendbuf), stdin)) {
        // rimuovo eventuale '\n'
        size_t len = strlen(sendbuf);
        if (len && sendbuf[len-1]=='\n') sendbuf[len-1]=0;

        if (strcmp(sendbuf, "exit")==0) break;

        if (dry_run) {
            printf("[DRY] WOULD SEND: %s\n", sendbuf);
            continue;
        }

        // invio comando
        send(sock, sendbuf, strlen(sendbuf), 0);
        send(sock, "\n", 1, 0);

        // ricevo risposta (piccolo timeout implicito)
        ssize_t n;
        while ((n = recv(sock, recvbuf, sizeof(recvbuf)-1, MSG_DONTWAIT)) > 0) {
            recvbuf[n] = 0;
            fputs(recvbuf, stdout);
        }
    }

    if (!dry_run) close(sock);
    return 0;
}
