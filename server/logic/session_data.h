#include <account.h>

enum SessionState
{
    UNAUTHENTICATED,
    AUTHENTICATED,
};

typedef struct SessionData_t
{
    account *user;
    int socket;
    enum SessionState state;
} SessionData;