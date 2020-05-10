#include "helpers.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

/**
 * Display a prompt requesting the user's credentials, then build a JSON object
 * containing them;
 * Used for both the register and login command;
 *
 * @return      a char array converted from the JSON object;
 */
char *build_credentials_json() {
    json credentials;
    string credentials_string, username, password;
    char *credentials_char = (char *) malloc(50 * sizeof(char));

    /* Read the username and the password */
    cout << "username=";
    getline(std::cin, username);
    cout << "password=";
    getline(std::cin, password);

    /* Add the username and password to the credentials JSON */
    credentials["username"] = username;
    credentials["password"] = password;

    /* Convert the JSON to std::string and the string to a char array */
    credentials_string = credentials.dump();
    strcpy(credentials_char, credentials_string.c_str());

    return credentials_char;
}

/**
 * Parse the cookies field in the response and retrieve the first one;
 *
 * @param response      the answer received from the server;
 * @return              the cookie extracted from the response;
 */
string parse_cookie(const string &response) {
    string cookie;
    const string cookie_prompt = "Set-Cookie:";
    size_t index = response.find(cookie_prompt) + cookie_prompt.length();

    while (response[index] != ';') {
        cookie.push_back(response[index]);
        index++;
    }

    return cookie;
}

/**
 * Display a prompt for the user to insert input, until the input is not an
 * empty string;
 *
 * @param field         the field where the input is stored;
 * @param prompt        the prompt to be displayed;
 */
void get_field(string &field, const string &prompt) {
    while (field.empty()) {
        cout << prompt;
        getline(std::cin, field);
    }
}

/**
 * Check if the provided string contains only digits (is a number);
 *
 * @param string        the string to be checked;
 * @return              true if the string is a number;
 */
bool check_number(const string &string) {
    for (char i : string)
        if (!isdigit(i))
            return false;

    return true;
}

/**
 * Parse the response from the server, extract the JSON and retrieve the error
 * message;
 *
 * @param response      the server response;
 * @return              the error message;
 */
string parse_error(char *response) {
    json error_json;

    string error_string(basic_extract_json_response(response));
    error_json = json::parse(error_string);

    return error_json["error"];
}

/**
 * Check if the given command can be executed, depending on the user login state
 *
 * @param logged_in         a bool representing the login status;
 * @param command_index     the index of the command from the map;
 * @return                  true if the command can be executed; false otherwise
 */
bool check_user_logged_in(bool logged_in, int command_index) {
    if (command_index == 1) {
        if (logged_in) {
            cout << "You can't register an account while you are logged in!" <<
                 endl;
            return false;
        }

        return true;
    }

    if (command_index == 2) {
        if (logged_in) {
            cout << "You are already logged in!" << endl;
            return false;
        }

        return true;
    }

    if (!logged_in && command_index != 9) {
        cout << "You can't issue this command unless you are logged in!" <<
             endl;
        return false;
    }

    return true;
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(const char *host_ip, int port_number, int ip_type, int
socket_type) {
    struct sockaddr_in serv_addr{};
    int sockfd = socket(ip_type, socket_type, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(port_number);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);

    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd) {
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);

        header_end = buffer_find(&buffer, HEADER_TERMINATOR,
                                 HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            int content_length_start = buffer_find_insensitive(&buffer,
                                                               CONTENT_LENGTH,
                                                               CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, nullptr,
                                    10);
            break;
        }
    } while (true);
    size_t total = content_length + (size_t) header_end;

    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str) {
    return strstr(str, "{\"");
}
