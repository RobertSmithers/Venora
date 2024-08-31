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
#include "networking/networking.h"
#include "networking/certificates.c"
#include "networking/web-utils.h"
#include "logic/actions.h"
#include "logic/logic.h"
#include "database/accessor.h"
#include "log.h"

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
    int c_sock = sock;

    // Create session data
    SessionData *session = (SessionData *)malloc(sizeof(SessionData));

    session->socket = c_sock;
    session->user = (account *)malloc(sizeof(account));
    session->state = UNAUTHENTICATED;

    while (!socket_closed(session->socket))
    {
        // TODO: Protobuff here for request_type
        // First two bytes should be the req type
        uint16_t request_type;
        recv(session->socket, &request_type, REQ_RESP_TYPE_SIZE, 0);
        request_type = ntohs(request_type);

        printf("Got a request\n");
        switch (request_type)
        {
        case REQUEST_REGISTER:
            handle_registration(session);
            break;
        case REQUEST_LOGIN:
            handle_login(session);
            break;
        case REQUEST_GET_STRIKE_PACKS:
            handle_list_strike_packs(session);
            break;
        default:
            printf("Unrecognized request type: %d\n", request_type);
            send_response_invalid(session->socket);
            break;
        }
        // Any code run here will run even in events of error responses
    }

    // As long as it's past the 'UNAUTHENTICATED' state, user account will be populated
    if (session->state != UNAUTHENTICATED)
    {
        printf("Free username\n");
        free(session->user->username);
        printf("Free token\n");
        free(session->user->token);
    }
    printf("Free user\n");
    free(session->user);

    close(sock);

    printf("Client disconnected.\n\n");
    client_count--;
}

int main()
{
    setbuf(stdout, NULL);
    log_debug("I am a server. I am A SERVER. I am A SERVER. I AM A SERVER!\n");

    // Before setting up server connection, verify connection to the database
    // This includes verifying SSL certs
    if (!is_db_online())
    {
        perror("Database is offline or unreachable");
        exit(EXIT_FAILURE);
    }

    int server_socket,
        client_socket;
    // This includes verifying SSL certs
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

    printf("Listening on port %d...\n", PORT);

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
        printf("TODO: Thread out to handle_client\n\n");
        handle_client(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}