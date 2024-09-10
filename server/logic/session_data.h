#include <openssl/ssl.h>

#include "account.h"

enum SessionState
{
    UNAUTHENTICATED,
    AUTHENTICATED,
};

typedef struct SessionData_t
{
    account *user;
    int socket;
    SSL *ssl;
    enum SessionState state;
} SessionData;