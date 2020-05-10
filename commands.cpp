#include "commands.h"

const char *application_json = "application/json";
const char *HOST = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";

/**
 * Build the POST request message, containing the credentials JSON, then send it
 * to the server; Upon reception of the response, check if the username is
 * available;
 *
 * @param sockfd    The socket where the request should be sent;
 * @param cookies   The vector of cookies attached in the request;
 */
void register_command(int sockfd, const vector<string> &cookies) {
    const char *url = "/api/v1/tema/auth/register";
    const string success = "201 Created";
    char *response;

    /* Create a JSON object with the credentials provided by the user and
     * convert it to a char array */
    char *credentials = build_credentials_json();

    /* Build the POST request message, send it to the server and retrieve the
     * response */
    response = compute_request(sockfd, "POST", HOST, url, application_json,
                               credentials, cookies, "");
    string response_string(response);

    /* Check if the registration was successful */
    if (response_string.find(success) != string::npos) {
        cout << "Registered successfully!" << endl;
        free(response);
        return;
    }

    /* If the request returned an error, parse it and display the content */
    cout << parse_error(response) << endl;
    free(response);
}

/**
 * Build a POST request with the credentials provided, then send it to the
 * server; Extract a cookie if the login is successful;
 *
 * @return      true if the login was successful, false otherwise;
 */
bool login_command(int sockfd, vector<string> &cookies) {
    const char *url = "/api/v1/tema/auth/login";
    string authentication_cookie, token;
    const string success = "200 OK";
    char *response;

    /* Get the JSON format string */
    char *credentials = build_credentials_json();

    /* Compute the POST request, send it to the server and get the response */
    response = compute_request(sockfd, "POST", HOST, url, application_json,
                               credentials, cookies, "");
    string response_string(response);

    /* If the login was successful, extract the cookie from the response,
     * necessary to prove the authentication, then add it to the vector of
     * cookies */
    if (response_string.find(success) != string::npos) {
        cout << "Logged in successfully!" << endl;
        authentication_cookie = parse_cookie(response_string);
        cookies.push_back(authentication_cookie);
        free(response);
        return true;
    }

    cout << parse_error(response) << endl;
    free(response);
    return false;
}

/**
 * Send a GET message to the server to request access to the library; Receive a
 * JWT token to confirm access;
 *
 * @param token     a string passed by reference where the value of the token
 * will be stored;
 */
void enter_library_command(int sockfd, vector<string> &cookies, string &token) {
    const char *url = "/api/v1/tema/library/access";
    const string success = "200 OK";
    json token_json;
    char *response;

    response = compute_request(sockfd, "GET", HOST, url, nullptr, nullptr,
                               cookies, token);
    string response_string(response);

    /* If the request is successful, retrieve the JWT token */
    if (response_string.find(success) != string::npos) {
        string token_string(basic_extract_json_response(response));
        token_json = json::parse(token_string);
        token = token_json["token"];

        cout << "You now have access to the library!" << endl;
        cout << "JWT Token: " << token << endl;

        free(response);
        return;
    }

    cout << parse_error(response) << endl;
    free(response);
}

/**
 * Send a GET request, that returns an array of JSON objects, representing the
 * books; Parse the array and display book details in a user friendly format
 */
void get_books_command(int sockfd, const vector<string> &cookies, const
string &token) {
    const char *url = "/api/v1/tema/library/books";
    const string success = "200 OK";
    char *response;

    response = compute_request(sockfd, "GET", HOST, url, nullptr, nullptr,
                               cookies, token);
    string response_string(response);

    if (response_string.find(success) != string::npos) {
        int i = 0;
        json json_array;
        char *books_array = strstr(response, "[{\"");

        string books(books_array);
        json_array = json::parse(books);

        while (!json_array[i]["title"].empty()) {
            cout << "ID: " << json_array[i]["id"] << " -> Title: " <<
                 json_array[i]["title"] << endl;
            i++;
        }

        free(response);
        return;
    }

    cout << parse_error(response) << endl;
    free(response);
}

/**
 * Retrieve information about a book with the specified ID; Issue a GET request
 * to the URL corresponding to the ID to get the details;
 */
void get_book_command(int sockfd, const vector<string> &cookies, const string
&token) {
    string url = "/api/v1/tema/library/books/";
    const string success = "200 OK";
    char *response;
    string id;

    /* Check if the inserted ID is a number; If not, ask the user to insert
     * another ID until the ID is valid */
    get_field(id, "id=");
    while (!check_number(id)) {
        cout << "Invalid value for ID!" << endl;
        cout << "id=";
        getline(std::cin, id);
    }

    /* Append the ID to the URL */
    url.append(id);

    response = compute_request(sockfd, "GET", HOST, url.c_str(), nullptr,
                               nullptr, cookies, token);
    string response_string(response);

    if (response_string.find(success) != string::npos) {
        json book_json;
        char *details = basic_extract_json_response(response);
        string book_details(details);
        book_details.pop_back();

        book_json = json::parse(book_details);

        cout << "Title: " << book_json["title"] << endl;
        cout << "Author: " << book_json["author"] << endl;
        cout << "Publisher: " << book_json["publisher"] << endl;
        cout << "Genre: " << book_json["genre"] << endl;
        cout << "Page count: " << book_json["page_count"] << endl;

        free(response);
        return;
    }

    cout << parse_error(response) << endl;
    free(response);
}

/**
 * Ask for input from the user, representing the details about a new book that
 * should be added to the library; Add the data to a JSON object, then send a
 * POST request to add the book;
 */
void add_book_command(int sockfd, const vector<string> &cookies, const
string &token) {
    char *book_details = (char *) malloc(200 * sizeof(char));
    string title, author, genre, publisher, page_count;
    const char *url = "/api/v1/tema/library/books";
    const string success = "200 OK";
    char *response;
    json book_json;

    /* The get_field function asks for user input for each field until the
     * input is not an empty string */
    get_field(title, "title=");
    get_field(author, "author=");
    get_field(genre, "genre=");
    get_field(publisher, "publisher=");
    get_field(page_count, "page_count=");

    /* Check if the page count field is a number */
    while (!check_number(page_count)) {
        cout << "Invalid value for page count!" << endl;
        cout << "page_count=";
        getline(std::cin, page_count);
    }

    /* Add the details to the JSON object */
    book_json["title"] = title;
    book_json["author"] = author;
    book_json["genre"] = genre;
    book_json["publisher"] = publisher;
    book_json["page_count"] = stoi(page_count);

    string book_details_string = book_json.dump();
    strcpy(book_details, book_details_string.c_str());

    response = compute_request(sockfd, "POST", HOST, url, application_json,
                               book_details, cookies, token);
    string response_string(response);
    free(book_details);

    if (response_string.find(success) != string::npos) {
        cout << "Book was successfully added to the library!" << endl;
        free(response);
        return;
    }

    cout << parse_error(response) << endl;
    free(response);
}

/**
 * Send a DELETE request to the URL corresponding to the ID to delete the book;
 */
void delete_book_command(int sockfd, const vector<string> &cookies, const
string &token) {
    string url = "/api/v1/tema/library/books/";
    const string success = "200 OK";
    char *response;
    string id;

    /* Check if ID is number */
    get_field(id, "id=");
    while (!check_number(id)) {
        cout << "Invalid value for ID!" << endl;
        cout << "id=";
        getline(std::cin, id);
    }

    url.append(id);

    response = compute_request(sockfd, "DELETE", HOST, url.c_str(), nullptr,
                               nullptr, cookies, token);
    string response_string(response);

    if (response_string.find(success) != string::npos) {
        cout << "Book was successfully removed from the library!" << endl;
        free(response);
        return;
    }

    cout << parse_error(response) << endl;
    free(response);
}

/**
 * Issue a GET request to the logout URL;
 *
 * @return      false if the logout was successful, true otherwise; (the return
 * value will be used to determine if the user is logged in)
 */
bool logout_command(int sockfd, const vector<string> &cookies, const
string &token) {
    const char *url = "/api/v1/tema/auth/logout";
    const string success = "200 OK";
    char *response;

    response = compute_request(sockfd, "GET", HOST, url, nullptr, nullptr,
                               cookies, token);
    string response_string(response);

    if (response_string.find(success) != string::npos) {
        cout << "You were successfully logged out!" << endl;
        free(response);
        return false;
    }

    cout << parse_error(response) << endl;
    free(response);
    return true;
}