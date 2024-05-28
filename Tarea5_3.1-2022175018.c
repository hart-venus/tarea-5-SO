// Código del servidor de chat

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex para la lista de clientes

void broadcast_message(char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex); // bloquear el mutex
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_socket) {
            // enviar mensaje a todos los clientes excepto al que lo envió
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex); // liberar el mutex
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // mientras se reciban mensajes
        buffer[bytes_read] = '\0'; // terminar el mensaje
        broadcast_message(buffer, client_socket); // enviar mensaje a todos los clientes
    }

    close(client_socket); // cerrar el socket del cliente
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = 0; // quitar el socket de la lista de clientes
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex); // liberar el mutex
    free(arg); 
    return NULL;
}

int main() {
    int server_socket, new_socket; // sockets para el servidor y los clientes
    struct sockaddr_in server_addr, client_addr; // direcciones del servidor y los clientes
    socklen_t addr_len = sizeof(client_addr); // tamaño de la dirección del cliente
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)); // enlazar el socket al puerto
    listen(server_socket, 10); // escuchar en el puerto

    printf("Servidor escuchando en el puerto %d\n", PORT); // mensaje de inicio

    while (1) {
        // aceptar conexiones de clientes
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        pthread_mutex_lock(&clients_mutex); // bloquear el mutex
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                int *new_sock = malloc(sizeof(int));
                *new_sock = new_socket;
                pthread_create(&tid, NULL, handle_client, (void *)new_sock);
                // crear un hilo para manejar al cliente
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex); // liberar el mutex
    }

    close(server_socket); // cerrar el socket del servidor
    return 0;
}
