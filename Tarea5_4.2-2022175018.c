// Cliente que solicita un archivo al servidor y muestra su contenido en pantalla
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void write_file(int sockfd) {
    // función para recibir un archivo línea por línea de un socket
    int n;
    char buffer[BUFFER_SIZE];
    while ((n = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        printf("%s", buffer);
        memset(buffer, 0, BUFFER_SIZE);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char filename[BUFFER_SIZE];

    // crear socket del cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // conseguir el nombre del archivo
    printf("Ingrese el nombre del archivo a solicitar: ");
    scanf("%s", filename);
    send(client_socket, filename, strlen(filename), 0);

    printf("Contenido del archivo:\n");
    write_file(client_socket);
    // cerrar el socket del cliente
    close(client_socket);
    return 0;
}
