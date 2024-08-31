#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <curl/curl.h>
#include <json-c/json.h>

const char *get_base_url();

// Function prototypes for HTTP GET and POST
struct json_object *http_get(const char *url, json_object *json_data);
struct json_object *http_post(const char *url, json_object *json_data);

char *url_encode(const char *str);
char *json_object_to_url_encoded_str(struct json_object *json_obj);

#endif // HTTP_CLIENT_H
