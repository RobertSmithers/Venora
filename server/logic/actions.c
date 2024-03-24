#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <actions.h>
#include <account.h>
#include "../database/token.h"

bool register_user(char *username, uint16_t username_len, char *token)
{
    /*
    registers the user with username and returns an access token.
    Returns true on success, false if user exists or failure
    */

    // TODO: Setup db and authenticate the username
    // TODO: If username taken, return false

    // TODO: Generate random token
    // char *gend_token = "abcdefghijklmnopqrstuvwxyz123456";

    // Copy token into struct
    // memcpy(token, gend_token, TOKEN_SIZE);
    generate_token(token, TOKEN_SIZE);

    return true;
}

void login(char *username, uint16_t username_len, char *token, uint16_t token_len)
{
}

void get_strike_packs()
{
}