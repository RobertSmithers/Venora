// This might be redundant, but may add more cert functionality in the future
#ifndef CERTIFICATES_H
#define CERTIFICATES_H

#include <curl/curl.h>

void init_ssl(CURL *curl);
void init_ssl_to_db_api(CURL *curl);

#endif // CERTIFICATES_H
