#include <stdio.h>
#include <json-c/json.h>
// #include <libpq-fe.h>

struct json_object *db_get_categories();

struct json_object *db_get_user(const char *username);
struct json_object *db_login_user(const char *username, const char *token);
struct json_object *db_insert_user(const char *username, const char *password, const char *token);
struct json_object *db_get_strike_packs(const char *username);

struct json_object *db_get_health();
