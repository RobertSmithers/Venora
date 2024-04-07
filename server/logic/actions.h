#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

bool register_user(PGconn *db_conn, char *username, uint16_t username_len, char *token);
bool login(PGconn *db_conn, char *username, char *token);
void get_strike_packs(PGconn *db_conn);
