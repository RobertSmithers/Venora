#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "networking/web-utils.h"

#define DB_NAME "VenoraDB"
#define DB_USER "postgres"
#define DB_HOST "db"

/* Function to retrieve categories via HTTP GET */
struct json_object *db_get_categories()
{
    // struct json_object *response = http_get("/categories", NULL);
    // printf("GET Response: %s\n", json_object_to_json_string(response));
    // return json_object_to_json_string(response);
    return http_get("/categories", NULL);
}

/* Function to retrieve user details via HTTP GET */
struct json_object *db_get_user(const char *username)
{
    json_object *data = json_object_new_object();
    json_object_object_add(data, "username", json_object_new_string(username));
    // const char *json_str = json_object_to_json_string(jobj);
    // char *result = strdup(json_str); // Make a copy of the string
    // json_object_put(jobj);           // Free the json object

    return http_get("/user", data);
    // char url[256];
    // sprintf(url, "/user?username=%s", username);
    // return http_get(url);
}

struct json_object *db_login_user(const char *username, const char *password)
{
    json_object *data = json_object_new_object();
    json_object_object_add(data, "username", json_object_new_string(username));
    json_object_object_add(data, "password", json_object_new_string(password));
    return http_post("/login_user", data);
}

/* Function to insert a new user via HTTP POST */
struct json_object *db_insert_user(const char *username, const char *password, const char *token)
{
    json_object *data = json_object_new_object();
    json_object_object_add(data, "username", json_object_new_string(username));
    json_object_object_add(data, "password", json_object_new_string(password));
    json_object_object_add(data, "token", json_object_new_string(token));
    return http_post("/user", data);
    // struct json_object *json = json_object_new_object();
    // json_object_object_add(json, "username", json_object_new_string(username));
    // json_object_object_add(json, "token", json_object_new_string(token));

    // const char *json_payload = json_object_to_json_string(json);
    // return http_post("/user", json_payload);
}

/* Function to retrieve strike packs via HTTP GET */
struct json_object *db_get_strike_packs(const char *username)
{
    json_object *data = json_object_new_object();
    json_object_object_add(data, "username", json_object_new_string(username));
    return http_get("/packs", data);
    ;
    // char url[256];
    // sprintf(url, "/packs/%s", username);
    // return http_get(url);
}

/* Function to check database status (used to verify connectivity and/or report state)*/
struct json_object *db_get_health()
{
    // return "";
    // // return http_get("/health");
    // struct json_object *response = http_get("/health", NULL);
    // printf("GET Response: %s\n", json_object_to_json_string(response));
    // return (char *)json_object_to_json_string(response);
    return http_get("/health", NULL);
}