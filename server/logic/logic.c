/*
Contains the main logic that the application uses. Calls the appropriate actions from actions.c
*/

#include <logic.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "networking/networking.h"
#include "networking/utils.h"
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
    if (session->state != UNAUTHENTICATED)
    {
        // TODO: Delete accounts not logged into
        //      after some threshold timeout period
        char *msg = "Please log out of your current account before registering";
        send_response_failure(session->socket, msg);
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

    // account *user = session->user;
    char *req_username = (char *)malloc((username_len + 1) * sizeof(char)); // Add space for null terminator char
    recv(session->socket, req_username, username_len, 0);

    req_username[username_len] = '\0';

    printf("req_username is %s (size %d)\n", req_username, username_len);

    char *token = (char *)malloc(TOKEN_SIZE * sizeof(char));

    // Do user registration
    if (register_user(session->db_conn, req_username, username_len, token))
    {
        printf("User %s registered successfully\n", req_username);
        printf("Sending token %s\n", token);
        DataBlock block[1][1] = {{&token, sizeof(token)}};
        send_response_success_data(session->socket, 1, (DataBlock **)block);
    }
    else
    {
        char msg[40 + username_len];
        sprintf(msg, "Failed to register. Username %s is taken.", req_username);
        send_response_failure(session->socket, msg);
    }
    free(req_username);
    free(token);
}

void handle_login(SessionData *session)
{
    // Can only login if not already logged in
    // UNAUTH is the only state that is not considered authed (future states are implicitly "authed")
    if (session->state != UNAUTHENTICATED)
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
    printf("req_username is %s (size %d)\n", req_username, username_len);

    uint16_t token_len = get_req_varlen_value(session->socket);

    // Invalid token size = invalid login attempt
    if (token_len != TOKEN_SIZE)
    {
        // Flush the rest of the socket (to prevent misinterpreting remainder as new request)
        flush_socket(session->socket);
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
        free(req_username);
        free(req_token);
        send_response_failure(session->socket, "");
        return;
    }
}

void handle_list_strike_packs(SessionData *session)
{
    // Request requires authentication
    if (session->state != UNAUTHENTICATED)
    {
        send_response_failure(session->socket, "You must be logged in to see available strike packs");
        return;
    }

    StrikePackList *available_strike_packs = list_strike_packs(session->db_conn, session->user->username);
    // TODO: Account for failure in the db_accessor. Currently we just quit the server program... can't do this for prod
    if (NULL == available_strike_packs)
    {
        // Success even when no strike packs are returned (not authorized or doesn't exist)
        // Data field should be blank
        send_response_success_data(session->socket, 0, NULL);
    }
    else
    {
        DataBlock **blocks = uniform_list_to_data_blocks((void **)available_strike_packs->strike_packs,
                                                         available_strike_packs->count,
                                                         sizeof(StrikePack));
        send_response_success_data(session->socket,
                                   available_strike_packs->count,
                                   blocks);
        free_data_blocks(blocks, available_strike_packs->count);
    }
    return;
}