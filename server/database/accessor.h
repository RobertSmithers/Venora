#include <stdio.h>
#include <libpq-fe.h>

PGconn *connect_to_db();
PGresult *get_categories();
PGresult *get_user(PGconn *conn, char *username);
int insert_user(PGconn *conn, char *username, char *token);
