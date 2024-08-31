#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <json-c/json.h>

#include "../database/db_structs.h"

bool json_str_field_cmp(json_object *data, char *key, char *val);

bool register_user(char *username, uint16_t username_len, char *password, uint16_t password_len, char *token);
const char *login_user(const char *username, const char *password);

bool is_db_online();

StrikePackList *list_strike_packs(char *usernam);
