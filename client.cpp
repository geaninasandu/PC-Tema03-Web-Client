#include "helpers.h"
#include "commands.h"

int main(int argc, char *argv[]) {
    string command, username, password, jwt_token;
    const char *SERVER_IP = "3.8.116.10";
    bool logged_in = false;
    vector<string> cookies;
    const int PORT = 8080;
    int sockfd;

    /* Create a map that associates the possible commands to an index */
    map<string, int> commands_map = {
            {"register",      1},
            {"login",         2},
            {"enter_library", 3},
            {"get_books",     4},
            {"get_book",      5},
            {"add_book",      6},
            {"delete_book",   7},
            {"logout",        8},
            {"exit",          9}
    };

    while (true) {
        /* Read the command */
        getline(std::cin, command);

        /* If the inserted command doesn't exist in the map, display an error
         * message and ask the user to insert another command, until provided
         * with a valid one */
        while (commands_map.find(command) == commands_map.end()) {
            cout << "Invalid command. Please insert another!" << endl;
            getline(std::cin, command);
        }

        /* Get the index of the command from the map */
        int command_index = commands_map.at(command);

        /* Check if the user login status is suitable for the command;
         * If not suitable, display an error message and ask for another
         * command */
        if (!check_user_logged_in(logged_in, command_index))
            continue;

        /* Open the socket connection */
        sockfd = open_connection(SERVER_IP, PORT, AF_INET, SOCK_STREAM);

        /* For each index in the map, call the corresponding function of the
         * command */
        switch (command_index) {

            case 1:     /* register */
                register_command(sockfd, cookies);
                break;

            case 2:     /* login */
                logged_in = login_command(sockfd, cookies);
                break;

            case 3:     /* enter_library */
                enter_library_command(sockfd, cookies, jwt_token);
                break;

            case 4:     /* get_books */
                get_books_command(sockfd, cookies, jwt_token);
                break;

            case 5:     /* get_book */
                get_book_command(sockfd, cookies, jwt_token);
                break;

            case 6:     /* add_book */
                add_book_command(sockfd, cookies, jwt_token);
                break;

            case 7:     /* delete_book */
                delete_book_command(sockfd, cookies, jwt_token);
                break;

            case 8:     /* logout */
                logged_in = logout_command(sockfd, cookies, jwt_token);
                break;

            case 9:     /* exit */
                cout << "Connection closed!" << endl;
                close_connection(sockfd);
                return 0;

            default:
                break;
        }
    }
}
