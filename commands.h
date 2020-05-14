#ifndef COMMANDS_H
#define COMMANDS_H

#include "helpers.h"
#include "requests.h"

using namespace std;
using json = nlohmann::json;

void register_command(int sockfd, const vector<string> &cookies);

bool login_command(int sockfd, vector<string> &cookies);

void enter_library_command(int sockfd, vector<string> &cookies, string &token);

void get_books_command(int sockfd, const vector<string> &cookies,
                       const string &token);

void get_book_command(int sockfd, const vector<string> &cookies, const string
&token);

void add_book_command(int sockfd, const vector<string> &cookies, const
string &token);

void delete_book_command(int sockfd, const vector<string> &cookies, const
string &token);

bool logout_command(int sockfd, vector<string> &cookies, string &token);

#endif //COMMANDS_H
