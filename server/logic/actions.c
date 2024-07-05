#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
// #include <libpq-fe.h>
#include <json-c/json.h>

#include "actions.h"
#include "account.h"
#include "database/accessor.h"
#include "database/token.h"

/* Returns true if db is online & reachable, false otherwise */
bool is_db_online()
{
    char *resp = get_health();
    // API not reachable
    if (NULL == resp)
    {
        return false;
    }

    // Parse the JSON response
    struct json_object *parsed_json;
    struct json_object *status;

    parsed_json = json_tokener_parse(resp);
    if (parsed_json == NULL)
    {
        printf("WARNING -- Parse error for health json... Also I need to add actual logging :)\n");
        free(resp);
        return false;
    }

    // Check the status field for "ok"
    if (json_object_object_get_ex(parsed_json, "status", &status))
    {
        const char *status_str = json_object_get_string(status);

        if (strcmp(status_str, "ok") == 0)
        {
            json_object_put(parsed_json);
            free(resp);
            return true; // Online
        }
    }

    // Placeholder for other statuses
    // ex: some keyword for different health indications (out of sync, overheating..)

    // Default to offline
    json_object_put(parsed_json);
    free(resp);
    return false;
}

/*
    registers the user with username and writes to an access token.
    Returns true on success, false if user exists or failure
*/
bool register_user(char *username, uint16_t username_len, char *token)
{
    char *resp = get_user(username);
    if (resp)
    {
        struct json_object *parsed_json = json_tokener_parse(resp);
        printf("Response: %s\n", json_object_to_json_string(parsed_json));
        json_object_put(parsed_json);
        free(resp);
    }
    else
    {
        printf("Failed to get response\n");
        return false;
    }
    int numRows = 0;

    // If there is a nonzero number of rows (should be 0 or 1), user already exists
    if (numRows != 0)
    {
        return false;
    }

    // Generate user's random token and store in the database
    generate_token(token, TOKEN_SIZE);
    char *insert_resp = insert_user(username, token);
    if (insert_resp)
    {
        struct json_object *parsed_json2 = json_tokener_parse(insert_resp);
        printf("Response: %s\n", json_object_to_json_string(parsed_json2));
        json_object_put(parsed_json2);
        free(insert_resp);
    }
    else
    {
        printf("Failed to get insert user response\n");
        return false;
    }

    int row_ct = 1;
    if (row_ct == 0)
    {
        return false;
    }
    else if (row_ct != 1)
    {
        fprintf(stderr, "Register user insert operation modified multiple rows");
        return false;
    }

    return true;
}

/*
    Authenticates the user with username and token
    Returns true on success, false if invalid or failure
*/
bool login(char *username, char *token)
{
    // PGresult *pg_user_data = get_user(db_conn, username);
    // int numRows = PQntuples(pg_user_data);

    // // If there are no rows, user doesn't exist
    // if (numRows == 0)
    // {
    //     PQclear(pg_user_data);
    //     return false;
    // }

    // // Parse out username and token to compare
    // char *pg_username = PQgetvalue(pg_user_data, 0, 0);
    // char *pg_token = PQgetvalue(pg_user_data, 0, 1);

    // // Free the data
    // PQclear(pg_user_data);

    // // Should only need to compare token, but get_user function return values may change before release
    // if (strncmp(pg_username, username, MAX_USERNAME_LEN) == 0 &&
    //     strncmp(pg_token, token, TOKEN_SIZE) == 0)
    // {
    //     return true;
    // }

    return false;
}

/*
    Gets the strike packs from the database that this user can view
    Returns a list of StrikePack objects or NULL if there are none
*/
StrikePackList *list_strike_packs(char *username)
{
    // PGresult *pg_data = get_strike_packs(db_conn, username);
    // if (NULL == pg_data)
    // {
    //     printf("Error accessing strike pack data from database\n");
    //     return NULL;
    // }
    // int numRows = PQntuples(pg_data);

    // // If there are no rows, user cannot see any strike packs or none exist
    // if (numRows == 0)
    // {
    //     PQclear(pg_data);
    //     return NULL;
    // }

    // StrikePackList *p_splist = (StrikePackList *)malloc(sizeof(StrikePackList));
    // p_splist->strike_packs = (StrikePack **)malloc(sizeof(StrikePack *) * numRows);
    // for (int i = 0; i < numRows; i++)
    // {
    //     printf("Strike Name: %s, Strike Description: %s, Category ID: %s\n",
    //            PQgetvalue(pg_data, i, 0),
    //            PQgetvalue(pg_data, i, 1),
    //            PQgetvalue(pg_data, i, 2));
    //     // Create a StrikePack object and add it to the return pointer
    //     StrikePack *p_sp = (StrikePack *)malloc(sizeof(StrikePack));
    //     p_sp->name = PQgetvalue(pg_data, i, 0);
    //     p_sp->description = PQgetvalue(pg_data, i, 1);
    //     printf("TODO: THIS IS A CHAR* NOT AN INT AS EXPECTED\n");
    //     p_sp->category = PQgetvalue(pg_data, i, 2);
    //     p_splist->strike_packs[i] = p_sp;
    // }

    // // Free the data
    // PQclear(pg_data);

    // return p_splist;
    return NULL;
}