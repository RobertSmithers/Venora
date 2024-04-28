#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <libpq-fe.h>

#include "actions.h"
#include "account.h"
#include "database/accessor.h"
#include "database/token.h"

/*
    registers the user with username and writes to an access token.
    Returns true on success, false if user exists or failure
*/
bool register_user(PGconn *db_conn, char *username, uint16_t username_len, char *token)
{
    PGresult *pg_user_data = get_user(db_conn, username);
    int numRows = PQntuples(pg_user_data);

    // Free the data
    PQclear(pg_user_data);

    // If there is a nonzero number of rows (should be 0 or 1), user already exists
    if (numRows != 0)
    {
        return false;
    }

    // Generate user's random token and store in the database
    generate_token(token, TOKEN_SIZE);
    int row_ct = insert_user(db_conn, username, token);
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
bool login(PGconn *db_conn, char *username, char *token)
{
    PGresult *pg_user_data = get_user(db_conn, username);
    int numRows = PQntuples(pg_user_data);

    // If there are no rows, user doesn't exist
    if (numRows == 0)
    {
        PQclear(pg_user_data);
        return false;
    }

    // Parse out username and token to compare
    char *pg_username = PQgetvalue(pg_user_data, 0, 0);
    char *pg_token = PQgetvalue(pg_user_data, 0, 1);

    // Free the data
    PQclear(pg_user_data);

    // for (int i = 0; i < numRows; i++)
    // {
    //     printf("Category ID: %s, Category Name: %s\n",
    //            PQgetvalue(res, i, 0),
    //            PQgetvalue(res, i, 1));
    //     retVals[i] = (char *)malloc(sizeof(char *));
    // }
    // Should only need to compare token, but get_user function return values may change before release
    if (strncmp(pg_username, username, MAX_USERNAME_LEN) &&
        strncmp(pg_token, token, TOKEN_SIZE))
    {
        return true;
    }

    return false;
}

/*
    Gets the strike packs from the database that this user can view
    Returns a list of StrikePack objects or NULL if there are none
*/
StrikePackList *list_strike_packs(PGconn *db_conn, char *username)
{
    PGresult *pg_data = get_strike_packs(db_conn, username);
    int numRows = PQntuples(pg_data);

    // If there are no rows, user cannot see any strike packs or none exist
    if (numRows == 0)
    {
        PQclear(pg_data);
        return NULL;
    }

    StrikePackList *p_splist = (StrikePackList *)malloc(sizeof(StrikePackList));
    p_splist->strike_packs = (StrikePack **)malloc(sizeof(StrikePack *) * numRows);
    for (int i = 0; i < numRows; i++)
    {
        printf("Strike Name: %s, Strike Description: %s, Category ID: %s\n",
               PQgetvalue(pg_data, i, 0),
               PQgetvalue(pg_data, i, 1),
               PQgetvalue(pg_data, i, 2));
        // Create a StrikePack object and add it to the return pointer
        StrikePack *p_sp = (StrikePack *)malloc(sizeof(StrikePack));
        p_sp->name = PQgetvalue(pg_data, i, 0);
        p_sp->description = PQgetvalue(pg_data, i, 1);
        printf("TODO: THIS IS A CHAR* NOT AN INT AS EXPECTED\n");
        p_sp->category = PQgetvalue(pg_data, i, 2);
        p_splist->strike_packs[i] = p_sp;
    }

    // Free the data
    PQclear(pg_data);

    return p_splist;
}