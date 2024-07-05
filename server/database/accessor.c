#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "web-utils.h"

#define DB_NAME "VenoraDB"
#define DB_USER "postgres"
#define DB_HOST "db"

/* Function to retrieve categories via HTTP GET */
char *get_categories()
{
    return http_get("/categories");
}

/* Function to retrieve user details via HTTP GET */
char *get_user(const char *username)
{
    char url[256];
    sprintf(url, "/categories/%s", username);
    return http_get(url);
}

/* Function to insert a new user via HTTP POST */
char *insert_user(const char *username, const char *token)
{
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "username", json_object_new_string(username));
    json_object_object_add(json, "token", json_object_new_string(token));

    const char *json_payload = json_object_to_json_string(json);
    return http_post("/user", json_payload);
}

/* Function to retrieve strike packs via HTTP GET */
char *get_strike_packs(const char *username)
{
    char url[256];
    sprintf(url, "/packs/%s", username);
    return http_get(url);
}

/* Function to check database status (used to verify connectivity and/or report state)*/
char *get_health()
{
    return http_get("/health");
}