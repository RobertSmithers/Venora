/*
Contains the main logic that the application uses. Calls the appropriate actions from actions.c
*/

#include <logic.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "networking/networking.h"
#include "networking/schema.h"
#include "logic/actions.h"

uint16_t get_req_varlen_value(int socket)
{
    uint16_t data_len;
    recv(socket, &data_len, REQ_DATA_VARLEN_SIZE, 0);
    return ntohs(data_len);
}

void handle_registration(SessionData *session)
{
    if (session->state != NEW)
    {
        // TODO: Flush buffer here
        // Currently only supporting single registration per session
        // TODO: Allow registration anytime, but delete accounts not logged into
        //      after some threshold timeout period
        char *msg = "You have already registered. Please login.";
        send_response_failure(session->socket, msg);
        return;
    }

    // Extract username size and username string
    uint16_t username_len = get_req_varlen_value(session->socket);
    // recv(session->socket, &username_len, REQ_DATA_VARLEN_SIZE, 0);
    // username_len = ntohs(username_len);

    // Prevent buffer overflow or other memcpy error
    if (username_len > MAX_USERNAME_LEN)
    {
        printf("Failed to copy username. Username (%d chars) exceeds %d chars\n", username_len, MAX_USERNAME_LEN);
        char msg[41 + 4]; // 40 chars + 4 char max for username size
        sprintf(msg, "Username can be a maximum of %d characters", MAX_USERNAME_LEN);
        send_response_failure(session->socket, msg);
        return;
    }

    // TODO: If allowing multiple registrations per session, need to check & free user each time
    // TODO: Actually don't add this to the session until login
    account *user = session->user;
    user->username = (char *)malloc((username_len + 1) * sizeof(char)); // Add space for null terminator char
    recv(session->socket, session->user->username, username_len, 0);

    user->username[username_len] = '\0';

    printf("user->username is %s (size %d)\n", user->username, username_len);

    user->token = (char *)malloc(TOKEN_SIZE * sizeof(char));

    // Do user registration
    if (register_user(session->db_conn, user->username, username_len, user->token))
    {
        printf("User %s registered successfully\n", user->username);
        printf("Sending token %s\n", user->token);
        send_response_success_data(session->socket, user->token, TOKEN_SIZE);
        session->state = REGISTERED;
    }
    else
    {
        char msg[40 + username_len];
        sprintf(msg, "Failed to register. Username %s is taken.", user->username);
        send_response_failure(session->socket, msg);
        free(user->username);
        free(user->token);
    }
}

void handle_login(SessionData *session)
{
    // Can only login if not already logged in
    if (session->state != NEW && session->state != REGISTERED)
    {
        send_response_failure(session->socket, "Failed to login. Please log out of your current account first.");
        return;
    }

    // Extract username size and username string
    uint16_t username_len = get_req_varlen_value(session->socket);

    // Prevent buffer overflow or other memcpy error
    if (username_len > MAX_USERNAME_LEN)
    {
        printf("Failed to copy username. Username (%d chars) exceeds %d chars\n", username_len, MAX_USERNAME_LEN);
        char msg[41 + 4]; // 40 chars + 4 char max for username size
        sprintf(msg, "Username can be a maximum of %d characters", MAX_USERNAME_LEN);
        send_response_failure(session->socket, msg);
        return;
    }

    char *req_username = (char *)malloc((username_len + 1) * sizeof(char)); // Add space for null terminator char
    recv(session->socket, req_username, username_len, 0);

    req_username[username_len] = '\0';
    printf("user->username is %s (size %d)\n", req_username, username_len);

    uint16_t token_len = get_req_varlen_value(session->socket);
    // Invalid token size = invalid login attempt
    if (token_len != TOKEN_SIZE)
    {
        // Flush the rest of the socket (to prevent misinterpreting remainder as new request)
        // Disable blocking mode
        // flags = fcntl(session->socket);

        printf("Flushing socket data\n");
        flush_socket(session->socket);
        printf("Sending resp failure\n");
        send_response_failure(session->socket, "");
        return;
    }

    char *req_token = (char *)malloc(token_len * sizeof(char));

    // Extract username and token from packet
    recv(session->socket, req_token, token_len, 0);

    // Attempt to authenticate
    if (login(session->db_conn,
              req_username,
              req_token))
    {
        session->user->username = req_username;
        session->user->token = req_token;
        session->state = AUTHENTICATED;
        send_response_success(session->socket);
        return;
    }
    else
    {
        send_response_failure(session->socket, "");
        return;
    }
}

void handle_get_strike_packs(SessionData *session)
{
}