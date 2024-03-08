#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <actions.h>
#include <account.h>

bool register_user(char *username, uint16_t username_len, char *token)
{
    /*
    registers the user with username and returns an access token.
    Returns true on success, false if user exists or failure
    */
    printf("Registering %s with size %d\n", username, username_len);
    printf("TODO: Setup db and authenticate the username");
    // TODO: If username taken, return false

    // TODO: Generate random token
    char *gend_token = "abcdefghijklmnopqrstuvwxyz12345";

    // Copy token into struct
    memcpy(token, gend_token, TOKEN_SIZE);
    return true;
}

void login(char *username, uint16_t username_len, char *token, uint16_t token_len)
{
}

void get_strike_packs()
{
}