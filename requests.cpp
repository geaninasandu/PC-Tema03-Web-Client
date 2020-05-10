#include "requests.h"

/**
 * Traverse the vector of cookies and add the Cookie header to the request
 * message;
 *
 * @param message       the request message;
 * @param cookies       the vector of cookies;
 */
void add_cookies(char *message, const vector<string> &cookies) {
    if (!cookies.empty()) {
        string line = "Cookie:";

        for (int i = 0; i < cookies.size(); i++) {
            line.append(cookies[i]);

            if (i != cookies.size() - 1)
                line.append(";");
        }
        compute_message(message, line.c_str());
    }
}

/**
 * If the token is not an empty string, add it to the message header;
 *
 * @param message       the request message;
 * @param token         the JWT token;
 */
void add_jwt_token(char *message, const string &token) {
    if (!token.empty()) {
        string line = "Authorization: Bearer " + token;
        compute_message(message, line.c_str());
    }
}

/**
 * Build a request message, depending on the request type, then send it to the
 * server and retrieve the response;
 *
 * @param request_type      can be GET, POST or DELETE;
 * @return                  the response from the server;
 */
char *compute_request(int sockfd, const string &request_type, const char *HOST,
                      const char *url, const char *content_type, char *content,
                      const vector<string> &cookies, const string &token) {
    char *message, *response;

    /* Call the function that corresponds to the request type */
    if (request_type == "GET") {
        message = compute_get_request(HOST, url, cookies, token);
    } else if (request_type == "POST") {
        message = compute_post_request(HOST, url, content_type, content,
                                       cookies, token);
    } else {
        message = compute_delete_request(HOST, url, cookies, token);
    }

    send_to_server(sockfd, message);
    free(message);

    response = receive_from_server(sockfd);
    return response;
}

char *compute_get_request(const char *host, const char *url, const
vector<string> &cookies, const string &token) {
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    sprintf(line, "GET %s HTTP/1.1", url);

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    add_cookies(message, cookies);
    add_jwt_token(message, token);

    compute_message(message, "");
    return message;
}

char *compute_post_request(const char *host, const char *url, const char
*content_type, char *body_data, const vector<string> &cookies, const string
                           &token) {
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %zu", strlen(body_data));
    compute_message(message, line);

    add_cookies(message, cookies);
    add_jwt_token(message, token);

    compute_message(message, "");
    compute_message(message, body_data);

    free(line);
    return message;
}

char *compute_delete_request(const char *host, const char *url, const
vector<string> &cookies, const string &token) {
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    add_cookies(message, cookies);
    add_jwt_token(message, token);

    compute_message(message, "");
    return message;
}
