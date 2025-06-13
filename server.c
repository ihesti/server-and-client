// server.c v2
// Remote-Cmd Server con update/upgrade e dry-run

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 10
#define BUF_SZ  4096

static int dry_run = 0;
static int do_update = 0;

// Esegue (o stampa, in dry-run) un comando shell
static int run_cmd(const char *cmd) {
    if (dry_run) {
        printf("[DRY] %s\n", cmd);
        return 0;
    }
    printf("[RUN] %s\n", cmd);
    return system(cmd);
}

// Thread handler per ogni client
static void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);

    char buf[BUF_SZ];
    while (1) {
        ssize_t n = recv(client_fd, buf, BUF_SZ-1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        char *nl = strchr(buf, '\n');
        if (nl) *nl = '\0';

        // Se comando speciale "exit", chiudi connessione
        if (strcmp(buf, "exit") == 0) break;

        // Esegui il comando ricevuto
        FILE *cmd = popen(buf, "r");
        if (!cmd) {
            snprintf(buf, BUF_SZ, "ERROR: %s\n", strerror(errno));
            send(client_fd, buf, strlen(buf), 0);
            continue;
        }
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
    int opt;

    // parsing opzioni
    while ((opt = getopt(argc, argv, "udp:h")) != -1) {
        switch (opt) {
        case 'u':
            do_update = 1;
            break;
        case 'd':
            dry_run = 1;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'h':
        default:
            fprintf(stderr,
                "Usage: %s [-u] [-d] [-p port]\n"
                "  -u       apt-get update && upgrade -y\n"
                "  -d       dry-run (stampa comandi)\n"
                "  -p port  porta di ascolto (default 12345)\n"
                "  -h       this help\n",
                argv[0]);
            exit(opt=='h'?0:1);
        }
    }

    // se richiesto, aggiorna il sistema
    if (do_update) {
        run_cmd("sudo apt-get update -y");
        run_cmd("sudo apt-get upgrade -y");
    }

    // crea socket IPv6 (accetta anche IPv4-mapped)
    int sockfd, yes = 1;
    struct sockaddr_in6 sa = {
        .sin6_family = AF_INET6,
        .sin6_port   = htons(port),
        .sin6_addr   = in6addr_any
    };

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
    printf("🚀 Server listening on port %d (dry_run=%d)…\n", port, dry_run);

    // loop di accept + thread per client
    while (1) {
        struct sockaddr_storage ss;
        socklen_t sl = sizeof(ss);
        int *cfd = malloc(sizeof(int));
        if (!cfd) continue;

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
