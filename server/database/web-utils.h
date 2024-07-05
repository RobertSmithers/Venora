#include <stdlib.h>
#include <curl/curl.h>

#include "db_structs.h"

const char *get_base_url();
char *http_post(const char *endpoint, const char *json_payload);
char *http_get(const char *endpoint);
StrikePackList *parse_strike_packs(const char *json_response);