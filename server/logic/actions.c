/*
This whole file serves as an abstraction from our accessor functionality
This enables us to be able to modify logic in the future as we modify our schema and introduce new business logic
    without impacting the api given to logic.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
// #include <libpq-fe.h> // For unpacking postgres obj's
#include <json-c/json.h>

#include "actions.h"
#include "account.h"
#include "database/accessor.h"
#include "database/token.h"
#include "json-parser.h"
#include "log.h"

/* Returns true if db is online & reachable, false otherwise */
bool is_db_online()
{
    struct json_object *resp = db_get_health();
    // API not reachable or SSL error
    if (NULL == resp)
    {
        return false;
    }

    // Parse the JSON response
    // struct json_object *status;

    // Check the status field for "ok"
    if (json_str_field_eq(resp, "status", "ok"))
    {
        return true; // Online
    }
    else
    {
        log_error("Malformed response json\n");
        return false; // Offline/unhealthy
    }

    // if (json_object_object_get_ex(resp, "status", &status))
    // {
    //     const char *status_str = json_object_get_string(status);

    //     if (strcmp(status_str, "ok") == 0)
    //     {
    //         return true; // Online
    //     }

    //     // Placeholder for other statuses
    //     // ex: some keyword for different health indications (out of sync, overheating..)
    // }
    // else
    // {
    //     fprintf(stderr, "Malformed response json");
    // }

    // // Default to offline
    // return false;
}

/*
    registers the user with username and writes to an access token.
    Returns true on success, false if user exists or failure
*/
bool register_user(char *username, uint16_t username_len, char *password, uint16_t password_len, char *token)
{
    // First check if the user exists already
    struct json_object *resp = db_get_user(username);
    if (NULL == resp)
    {
        log_error("get_user response is NULL\n");
        return false;
    }

    struct json_object *error;
    // In the event of an error, log it and return failure
    if (json_object_object_get_ex(resp, "error", &error))
    {
        const char *error_str = json_object_get_string(error);
        log_error("%s\n", error_str);
        json_object_put(error);
        // json_object_put(resp);
        return false;
    }
    json_object_put(error);

    // Looking for "msg": "User not found"
    struct json_object *msg;
    if (!json_object_object_get_ex(resp, "msg", &msg))
    {
        // If there's no msg field, user exists or it's malformed
        // TODO: Check for malformed json for sake of logging
        json_object_put(msg);
        // json_object_put(resp);
        return false;
    }

    const char *msg_str = json_object_get_string(msg);

    if (strcmp(msg_str, "User not found") != 0)
    {
        // User exists, got some other message
        log_warn("Message: %s\n", msg_str);
        json_object_put(msg);
        // json_object_put(resp);
        return false;
    }

    json_object_put(msg);
    // json_object_put(resp);

    // User does not exist

    // Generate user's random access token and store in the database
    generate_token(token, TOKEN_SIZE);
    struct json_object *insert_resp = db_insert_user(username, password, token);
    if (NULL == insert_resp)
    {
        log_error("Failed to get insert user response\n");
        // json_object_put(insert_resp);
        return false;
    }
    log_info("Insert response: %s\n", json_object_to_json_string(insert_resp));

    return json_str_field_eq(insert_resp, "status", "success");
    // struct json_object *status;
    // if (!json_object_object_get_ex(insert_resp, "status", &status))
    // {
    //     // If there's no status field, assume failure
    //     json_object_put(status);
    //     // json_object_put(insert_resp);
    //     return false;
    // }

    // const char *status_str = json_object_get_string(status);
    // if (strcmp(status_str, "success") == 0)
    // {
    //     json_object_put(status);
    //     // json_object_put(insert_resp);
    //     return true;
    // }

    // log_warn("Insert response status: %s\n", status_str);
    // json_object_put(status);
    // // json_object_put(insert_resp);
    // return false;
}

/*
    Authenticates the user with username and password
    Returns a token on success, NULL on failure
*/
const char *login_user(const char *username, const char *password)
{
    struct json_object *resp = db_login_user(username, password);

    if (json_is_null_or_error(resp))
    {
        log_error("login_user response NULLed or errored\n");
        return false;
    }

    struct json_object *token;
    if (json_object_object_get_ex(resp, "access_token", &token))
    {
        const char *token_str = json_object_get_string(token);
        // TODO: Add our token to our session
        printf("TODO: Add token %s to session\n", token_str);
        json_object_put(token);
        return token_str;
    }

    struct json_object *msg;
    if (json_object_object_get_ex(resp, "msg", &msg))
    {
        const char *msg_str = json_object_get_string(msg);
        log_warn("%s\n", msg_str);
        return NULL;
    }

    log_warn("Malformed json encountered in response object\n");
    return NULL;
}

/*
    Gets the strike packs from the database that this user can view
    Returns a list of StrikePack objects or NULL if there are none
*/
StrikePackList *list_strike_packs(char *username)
{
    int pack_count;
    json_object *resp = db_get_strike_packs(username);

    if (json_is_null_or_error(resp))
    {
        log_error("received null or error response\n");
        return NULL;
    }

    // Expect outermost "count": int
    json_object *count;
    if (json_object_object_get_ex(resp, "count", &count))
    {
        // Get the num
        pack_count = json_object_get_int(count);

        // If there are no rows, user cannot see any strike packs or none exist
        if (pack_count == 0)
        {
            json_object_put(resp);
            return NULL;
        }
    }

    // There exists at least one strike pack
    struct json_object *strike_pack_arr;
    json_object_object_get_ex(resp, "strike_packs", &strike_pack_arr);

    StrikePackList *p_splist = (StrikePackList *)malloc(sizeof(StrikePackList));
    p_splist->strike_packs = (StrikePack **)malloc(sizeof(StrikePack *) * pack_count);
    for (int i = 0; i < pack_count; i++)
    {
        // json_object *name, *description, *category_name;
        struct json_object *strike_pack_json = json_object_array_get_idx(strike_pack_arr, i);

        // Create a StrikePack object and add it to the return pointer
        StrikePack *p_sp = (StrikePack *)malloc(sizeof(StrikePack));
        if (set_strike_pack_fields_from_json(p_sp, strike_pack_json))
        {
            p_splist->strike_packs[i] = p_sp;
        }
    }

    json_object_put(resp);
    return p_splist;
}