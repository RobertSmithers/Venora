#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <schema.h>

/*
Send a response to the client with the Response Type of Failure
Optionally include a message that will be displayed to the user
*/
bool send_response_failure(int sock, char *msg);

/*
Send a response to the client with the Response Type of Success
Optional data field is included for special responses that require it
*/
bool send_response_success(int sock, void *data, uint16_t data_len);

/*
Send a response to the client with the Response Type of Invalid Request
*/
bool send_response_invalid(int sock);