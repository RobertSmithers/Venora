#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

// TODO: Handle mutex/concurrency
short client_count = 0;

void handle_client(int sock) {
    printf("Deal with talking to the client in this thread");
    char buffer[BUFFER_SIZE];
        ssize_t bytes_received;

        // Receive data from the client
        while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
            printf("Received from client: %.*s", (int)bytes_received, buffer);

            // TODO: Business logic
        }

        close(sock);

        printf("Client disconnected.\n");
        client_count--;
}

int main() {
    printf("I am a server. I am A SERVER. I am A SERVER. I AM A SERVER!\n");

    int server_socket, client_socket;

    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int PORT = 9393;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Socket listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Listen for clients
    while (1) {

        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len)) == -1) {
            perror("Socket accept failed");
            continue;
        }

        printf("Client connected: %s\n", inet_ntoa(client_address.sin_addr));
        client_count++;
        printf("Thread out to handle_client\n");
        handle_client(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}