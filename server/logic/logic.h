#include <stdio.h>
#include <stdlib.h>

#include <session_data.h>

/*
Carries out logic to attempt to register a user
Sends SUCCESS on success
or one of FAILURE, INVALID_REQ, or SERVER_ERROR otherwise
*/
void handle_registration(SessionData *session);

/*
Carries out logic to attempt to authenticate a user
Sends SUCCESS_DATA with a token on success
or one of FAILURE, INVALID_REQ, SERVER_ERROR
*/
void handle_login(SessionData *session);

/*
Carries out logic to attempt to retrieve strike packs
Sends SUCCESS_DATA with strike pack data on success
or one of FAILURE, INVALID_REQ, SERVER_ERROR
*/
void handle_list_strike_packs(SessionData *session);
