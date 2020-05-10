#ifndef REQUESTS_H
#define REQUESTS_H

#include "helpers.h"

using namespace std;

void add_cookies(char *message, const vector<string> &cookies);

char *compute_request(int sockfd, const string &request_type, const char *HOST,
                      const char *url, const char *content_type, char *content,
                      const vector<string> &cookies, const string &token);

char *compute_get_request(const char *host, const char *url, const
std::vector<std::string> &cookies, const string &token);

char *compute_post_request(const char *host, const char *url, const char
*content_type, char *body_data, const vector<string> &cookies,
                           const string &token);

char *compute_delete_request(const char *host, const char *url, const
vector<string> &cookies, const string &token);

#endif //REQUESTS_H
