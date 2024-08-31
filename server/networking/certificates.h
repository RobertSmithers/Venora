// This might be redundant, but may add more cert functionality in the future
#ifndef CERTIFICATES_H
#define CERTIFICATES_H

#include <curl/curl.h>

void init_ssl(CURL *curl);

#endif // CERTIFICATES_H
