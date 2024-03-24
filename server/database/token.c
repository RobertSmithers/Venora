#include <stdio.h>
#include <stdlib.h>

#include "token.h"

void generate_token(char *token, uint8_t token_size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    int index, i;
    for (i = 0; i < token_size; ++i)
    {
        // Note: For proof-of-concept dev use rand() is fine, but should change to use dedicated library
        index = rand() % (sizeof(charset) - 1);
        token[i] = charset[index];
    }

    token[token_size] = '\0';
}