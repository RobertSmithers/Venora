#include <stdlib.h>

#define TOKEN_SIZE 32
#define MAX_USERNAME_LEN 100

// struct permissions
// {

// };

typedef struct account_t
{
    char *username;
    char *token; // Do we want an isAuthed bool or recheck token every time?
                 // TODO: If the former, should prevent spoofing with RSA/other crypto
    // struct permissions;
} account;
