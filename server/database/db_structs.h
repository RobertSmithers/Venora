#pragma once
#include <stdint.h>

typedef struct StrikePack_t
{
    const char *name;
    const char *category;
    const char *description;
} StrikePack;

typedef struct StrikePackList_t
{
    StrikePack **strike_packs;
    uint16_t count;
} StrikePackList;