// Codigo de cliente
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *arg) {
    int socket = *(int *)arg; // socket del cliente
    char buffer[BUFFER_SIZE]; // buffer para mensajes
    int bytes_read;

    while ((bytes_read = recv(socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_read] = '\0'; // terminar el mensaje
        printf("%s\n", buffer); // imprimir mensaje
    }

    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    pthread_t tid;
    char message[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0); // crear socket del cliente
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // dirección del servidor

    // conectar al servidor
    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_create(&tid, NULL, receive_messages, (void *)&client_socket);

    // enviar mensajes al servidor
    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
    return 0;
}
