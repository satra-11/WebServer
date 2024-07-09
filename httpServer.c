#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
...
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd == 0) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }