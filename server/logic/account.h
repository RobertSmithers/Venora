#include <stdlib.h>

#ifndef account_types
#define TOKEN_SIZE 32
#define MAX_USERNAME_LEN 100
#define account_types true

// struct permissions
// {

// };

typedef struct account_t
{
    char *username;
    char *token;
} account;
#endif
