// Servidor que recibe el nombre de un archivo y lo envía al cliente.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(FILE *fp, int sockfd) {
    // función para enviar un archivo línea por línea a un socket

    char data[BUFFER_SIZE] = {0};

    while (fgets(data, BUFFER_SIZE, fp) != NULL) { // leer línea por línea
        if (send(sockfd, data, strlen(data), 0) == -1) {
            // en caso de que la función send falle
            perror("Error al enviar el archivo.");
            exit(1);
        }
        // limpiar el buffer
        memset(data, 0, BUFFER_SIZE);
    }
}

int main() {
    int server_socket, new_socket; // sockets para el servidor y el cliente
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 10);

    printf("Servidor escuchando en el puerto %d\n", PORT);

    while (1) {
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (new_socket < 0) {
            perror("Error al aceptar la conexión.");
            exit(1);
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            perror("Error al recibir el nombre del archivo.");
            close(new_socket);
            continue;
        }
        buffer[bytes_received] = '\0'; // Asegurarse de que el buffer esté terminado

        printf("Solicitud de archivo recibida: %s\n", buffer);

        fp = fopen(buffer, "r");
        if (fp == NULL) {
            perror("Error al abrir el archivo.");
            close(new_socket);
            continue;
        }

        send_file(fp, new_socket);
        printf("Archivo enviado exitosamente.\n");

        fclose(fp);
        close(new_socket);
    }

    close(server_socket);
    return 0;
}
