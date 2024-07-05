#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../database/db_structs.h"

bool register_user(char *username, uint16_t username_len, char *token);
bool login(char *username, char *token);

bool is_db_online();

StrikePackList *list_strike_packs(char *usernam);
