#ifndef _HELPERS_
#define _HELPERS_

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include "buffer.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

#define BUFLEN 4096
#define LINELEN 1000

#include <vector>

using namespace std;

char *build_credentials_json();

string parse_cookie(const string &response);

string parse_error(char *response);

void get_field(string &field, const string& prompt);

bool check_number(const string& string);

bool check_user_logged_in(bool logged_in, int command_index);

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(const char *host_ip, int port_number, int ip_type, int
socket_type);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

#endif
