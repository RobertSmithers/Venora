#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#define DB_NAME "VenoraDB"
#define DB_USER "postgres"
#define DB_HOST "db"

/*
 * Establish a connection to the PostgreSQL database.
 *
 * Returns:
 *   A pointer to the PostgreSQL database connection (PGconn),
 *   or NULL if the connection attempt fails.
 */
PGconn *connect_to_db()
{
    // Get postgres connection info from env/globals
    char *db_pass = getenv("DB_PASSWORD");
    if (db_pass == NULL)
    {
        fprintf(stderr, "DB_PASSWORD environment variable not set\n");
        return NULL;
    }

    char conninfo[512];

    // db is our other service (w/in a docker container)
    sprintf(conninfo, "connect_timeout=%d dbname=%s host=%s user=%s password=%s port=%d", 15, DB_NAME, DB_HOST, DB_USER, db_pass, 5432);

    // Connect
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    return conn;
}

/*
 * Retrieve a list of strike categories from the database.
 *
 * Returns:
 *   A PGresult pointer containing the list of categories.
 *   It is the responsibility of the caller to free the result using PQclear().
 */
PGresult *get_categories(PGconn *conn)
{
    // Execute query and store result
    PGresult *res = PQexec(conn, "SELECT category_name FROM categories");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    return res;
}

/*
 * Retrieve the row of data for the user with name username
 *
 * Returns:
 *   A PGresult pointer containing 0 or 1 rows with the fields username and token.
 *   It is the responsibility of the caller to free the result using PQclear().
 */
PGresult *get_user(PGconn *conn, char *username)
{
    // const char *query_template = "SELECT username, token FROM account where username=$1";
    const char *param_values[1] = {username};
    // int param_lengths[1] = {strlen(username)};
    // int param_formats[1] = {0};

    // Execute query and store result
    PGresult *res = PQexecParams(conn,
                                 "SELECT username, token FROM account where username=$1",
                                 1,            /* Number of params */
                                 NULL,         /* Param types */
                                 param_values, /* Param values */
                                 NULL,
                                 NULL,
                                 0 /* Result format 0-text, 1-binary */
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    return res;
}

/*
 * Insert a new user into the database.
 *
 * Parameters:
 *   conn: A pointer to the PostgreSQL database connection.
 *   username: The username of the new user.
 *   token: The token associated with the new user.
 *
 * Returns:
 *   The number of rows affected by the insertion, or -1 if an error occurs.
 */
int insert_user(PGconn *conn, char *username, char *token)
{
    const char *query_template = "INSERT INTO account (username, token) VALUES ($1, $2)";
    const char *param_values[2] = {username, token};
    int param_lengths[2] = {strlen(username), strlen(token)};
    int param_formats[2] = {0, 0}; // inserting text

    PGresult *res = PQexecParams(conn, query_template, 2, NULL, param_values, param_lengths, param_formats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Failed to execute query: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    // Get num rows inserted
    int row_ct = atoi(PQcmdTuples(res));

    PQclear(res);

    return row_ct;
}