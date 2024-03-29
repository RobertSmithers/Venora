#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "networking/schema.h"
#include "networking/networking.c"
#include "logic/actions.h"
#include "logic/account.h"
#include "logic/state.h"

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

// TODO: Handle mutex/concurrency
short client_count = 0;

bool socket_closed(int sock)
{
    char buffer[1];
    ssize_t bytesReceived = recv(sock, buffer, sizeof(buffer), MSG_PEEK);

    if (bytesReceived <= 0)
    {
        // Graceful close == 0
        // Error or other close < 0
        return true;
    }

    // Connection is still open
    return false;
}

void handle_client(int sock)
{
    char buffer[BUFFER_SIZE];
    int c_sock = sock;
    enum SessionState curr_state = NEW;
    account *user = (account *)malloc(sizeof(user));

    while (!socket_closed(c_sock))
    {
        // TODO: Protobuff here for request_type
        // First two bytes should be the req type
        uint16_t request_type;
        recv(sock, &request_type, REQ_RESP_TYPE_SIZE, 0);
        request_type = ntohs(request_type);

        switch (request_type)
        {
        case REQUEST_REGISTER:

            if (curr_state != NEW)
            {
                // TODO: Flush buffer here
                // Currently only supporting single registration per session
                // TODO: Allow registration anytime, but delete accounts not logged into
                //      after some threshold timeout period
                char *msg = "You have already registered. Please login.";
                send_response_failure(c_sock, msg);
                continue;
            }

            // Extract username size and username string
            uint16_t username_len;
            recv(sock, &username_len, REQ_DATA_VARLEN_SIZE, 0);
            username_len = ntohs(username_len);

            // Prevent buffer overflow or other memcpy error
            if (username_len > MAX_USERNAME_LEN)
            {
                printf("Failed to copy username. Username (%d chars) exceeds %d chars\n", username_len, MAX_USERNAME_LEN);
                char msg[41 + 4]; // 40 chars + 4 char max for username size
                sprintf(msg, "Username can be a maximum of %d characters", MAX_USERNAME_LEN);
                send_response_failure(c_sock, msg);
                continue;
            }

            user->username = (char *)malloc(username_len * sizeof(char));
            recv(sock, user->username, username_len, 0);

            user->token = (char *)malloc(TOKEN_SIZE * sizeof(char));

            // Do user registration
            if (register_user(user->username, username_len, user->token))
            {
                printf("User %s registered successfully\n", user->username);
                printf("Sending token %s\n", user->token);
                send_response_success_data(c_sock, user->token, TOKEN_SIZE);
                curr_state = REGISTERED;
            }
            else
            {
                char msg[40 + username_len];
                sprintf(msg, "Failed to register. Username %s is taken.", user->username);
                send_response_failure(c_sock, msg);
                free(user->username);
                free(user->token);
            }
            break;
        case REQUEST_LOGIN:
            printf("TODO: Login here\n");
            break;
        case REQUEST_GET_STRIKE_PACKS:
            printf("TODO: Get strike packs here\n");
            break;
        default:
            printf("Unrecognized request type: %d\n", request_type);
            send_response_invalid(c_sock);
            break;
        }
    }

    // As long as it's past the 'NEW' state, user account will be populated
    if (curr_state != NEW)
    {
        free(user->username);
        free(user->token);
    }
    free(user);

    close(sock);

    printf("Client disconnected.\n\n");
    client_count--;
}

int main()
{
    printf("I am a server. I am A SERVER. I am A SERVER. I AM A SERVER!\n");

    int server_socket, client_socket;
    srand((unsigned int)time(NULL)); // Used for token generation

    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("Setting SO_REUSEADDR failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    int PORT = 9393;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("Socket listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Listen for clients
    while (1)
    {
        // TODO: Store socket: ip map in shared memory to prevent multiple connections from same IP
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) == -1)
        {
            perror("Socket accept failed");
            continue;
        }

        printf("Client connected: %s\n", inet_ntoa(client_address.sin_addr));
        client_count++;
        printf("Thread out to handle_client\n\n");
        handle_client(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}