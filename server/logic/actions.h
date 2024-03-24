#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

bool register_user(char *username, uint16_t username_len, char *token);
void login(char *username, uint16_t username_len, char *token, uint16_t token_len);
void get_strike_packs();
