#include <stdint.h>

typedef struct StrikePack_t
{
    char *name;
    char *category;
    char *description;
} StrikePack;

typedef struct StrikePackList_t
{
    StrikePack **strike_packs;
    uint16_t count;
} StrikePackList;