/* Parse json responses according to Venora specification */
#include <string.h>
#include <stdio.h>

#include "log.h"
#include "logic/json-parser.h"

bool json_str_field_eq(json_object *data, char *key, char *val)
{
    struct json_object *field;
    // Check for key
    if (json_object_object_get_ex(data, key, &field))
    {
        // Check for val
        const char *val_str = json_object_get_string(field);
        if (strcmp(val_str, val) == 0)
        {
            json_object_put(field);
            return true;
        }
    }

    json_object_put(field);
    return false;
}

bool json_is_null_or_error(json_object *data)
{
    if (NULL == data)
    {
        return true;
    }

    struct json_object *error;
    // In the event of an error, log it and return failure
    if (json_object_object_get_ex(data, "error", &error))
    {
        const char *error_str = json_object_get_string(error);
        log_error("%s\n", error_str);
        json_object_put(error);
        return true;
    }
    json_object_put(error);
    return false;
}

bool set_strike_pack_fields_from_json(StrikePack *p_sp, json_object *strike_pack_json)
{
    // Get name from json
    struct json_object *name;
    if (!json_object_object_get_ex(strike_pack_json, "name", &name))
    {
        printf("Error: missing name in strike pack field data\n");
        return false;
    }
    p_sp->name = json_object_get_string(name);

    struct json_object *desc;
    if (!json_object_object_get_ex(strike_pack_json, "description", &desc))
    {
        printf("Error: missing name in strike pack field data\n");
        return false;
    }
    p_sp->description = json_object_get_string(desc);

    struct json_object *category;
    if (!json_object_object_get_ex(strike_pack_json, "category", &category))
    {
        printf("Error: missing name in strike pack field data\n");
        return false;
    }
    p_sp->category = json_object_get_string(category);

    printf("Strike Name: %s, Strike Description: %s, Category ID: %s\n", p_sp->name, p_sp->description, p_sp->category);
    return true;
}
