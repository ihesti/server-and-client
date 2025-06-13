// server.c
// Remote-Cmd Server: ascolta, accetta comandi, li esegue e rimanda stdout/stderr

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 10
#define BUF_SZ  4096

static void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);

    char buf[BUF_SZ];
    while (1) {
        // leggi linea di comando
        ssize_t n = recv(client_fd, buf, BUF_SZ-1, 0);
        if (n <= 0) break;
        buf[n] = 0;
        // strip newline
        char *nl = strchr(buf, '\n');
        if (nl) *nl = 0;
        if (strcmp(buf, "exit") == 0) break;

        // esegui con popen()
        FILE *cmd = popen(buf, "r");
        if (!cmd) {
            snprintf(buf, BUF_SZ, "ERROR: %s\n", strerror(errno));
            send(client_fd, buf, strlen(buf), 0);
            continue;
        }
        // invia ogni riga
        while (fgets(buf, BUF_SZ, cmd)) {
            send(client_fd, buf, strlen(buf), 0);
        }
        pclose(cmd);
    }

    close(client_fd);
    return NULL;
}

int main(int argc, char **argv) {
    int port = 12345;
    if (argc > 1) port = atoi(argv[1]);

    int sockfd, yes = 1;
    struct sockaddr_in6 sa = { .sin6_family = AF_INET6, .sin6_port = htons(port) };

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if (bind(sockfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(sockfd, BACKLOG) < 0) {
        perror("listen"); exit(1);
    }
    printf("🚀 Server listening on port %d …\n", port);

    while (1) {
        struct sockaddr_storage ss;
        socklen_t sl = sizeof(ss);
        int *cfd = malloc(sizeof(int));
        *cfd = accept(sockfd, (struct sockaddr*)&ss, &sl);
        if (*cfd < 0) {
            perror("accept");
            free(cfd);
            continue;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, cfd);
        pthread_detach(tid);
    }

    close(sockfd);
    return 0;
}
