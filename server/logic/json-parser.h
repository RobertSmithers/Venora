
#include <stdbool.h>
#include <json-c/json.h>

#include "database/db_structs.h"

/* Checks that `key` field exists and has the string value of `val` */
bool json_str_field_eq(json_object *data, char *key, char *val);

/* Returns true if `data` is NULL or has an error field. Logs all errors */
bool json_is_null_or_error(json_object *data);

bool set_strike_pack_fields_from_json(StrikePack *sp, json_object *data);