#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
#include "utils.h"
#include "client_utils.h"

int logged_in = false;
clock_t RTT = 0;
pthread_mutex_t mutex_logged_in;

void *receive(void *arg) {
    int sockfd = *(int *)arg;
    char *buf = malloc(MAX_MSG_STR);
    while (true) {
        memset(buf, 0, MAX_MSG_STR);
        int status = read_from_stream(sockfd, buf);
        if (status == 1) {
            printf("Exitted receiver thread...\n");
            free(buf);
            return NULL;
        }
        /* if msg is a query ack */
        if (is_str_equal(buf, "12:", 3)) {
            int start = 0;
            int colon_count = 0;
            while (colon_count < 3) {
                if (buf[start] == ':') {
                    colon_count++;
                }
                start++;
            }
            for (; buf[start] != '\0'; start++) {
                fprintf(stdout, "%c", buf[start]);
            }
        }
        else if (is_str_equal(buf, "10:", 3)) {
            int start = 0;
            int colon_count = 0;
            unsigned char *sender = malloc(MAX_NAME);

            memset(sender, 0, MAX_NAME);
            while (colon_count < 2) {
                if (buf[start] == ':') {
                    colon_count++;
                }
                start++;
            }
            int i = 0;
            while (buf[start] != ':') {
                sender[i++] = buf[start++];
            }
            start++;
            print_blue_arrow();
            fprintf(stdout, "Message from %s: ", sender);
            for (; buf[start] != '\0'; start++) {
                fprintf(stdout, "%c", buf[start]);
            }
            fprintf(stdout, "\n");
            free(sender);
        }
        else {
            int colon_count = 0;
            int start = 0;
            while (colon_count < 3) {
                if (buf[start] == ':') {
                    colon_count++;
                }
                start++;
            }
            print_bold_arrow();
            fprintf(stdout, "Server message: ");
            for (; buf[start] != '\0'; start++) {
                fprintf(stdout, "%c", buf[start]);
            }
            fprintf(stdout, "\n");
        }
    }
    
}
int main(void) {
    printf("%s", black);
    printf("%s", bold);
    printf("Client running, start entering messages.\n");
    printf("%s", black);
    struct addrinfo hints;
    struct addrinfo *clientinfo;
    struct addrinfo *hostinfo;
    unsigned char *input = malloc(MAX_MSG_STR);
    unsigned char *buf = malloc(MAX_MSG_STR);
    unsigned char *id = malloc(MAX_NAME);
    int sockfd;
    int joined_session = false;
    pthread_mutex_init(&mutex_logged_in, NULL);
    memset(&hints, 0, sizeof(hints));
    memset(input, 0, MAX_MSG_STR);
    memset(buf, 0, MAX_MSG_STR);
    memset(id, 0, MAX_NAME);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    pthread_t receiver;
    
    // get user messages
    while (true) {
        memset(input, 0, MAX_MSG_STR);
        unsigned char c = 'a';
        int input_ptr = 0;
        // fprintf(stdout, "   > ");
        while (c != '\n') {
            c = getc(stdin);
            if (c == '\n') {
                break;
            }
            input[input_ptr++] = c;
        }

        // if not logged in and user requested something other than login, reject.
        if (!is_str_equal(input, "/login ", 7) && !is_str_equal(input, "/register ", 10) && !logged_in) {
            fprintf(stdout, "Please in first.\n");
            continue;
        }
        
        if (is_str_equal(input, "/login ", 7)) {
            if (logged_in) {
                print_error("Already logged in. Please log out first.");
                continue;
            }
            memset(id, 0, MAX_NAME);
            // clock_t before = clock();
            // int sec = val;
            // int usec = (val - sec) * 1e6;
            int response = handle_login(input, id);
            // clock_t after = clock() - prev;
            // RTT = ((double) after) / CLOCKS_PER_SEC;
            if (response != -1) {
                sockfd = response;
                logged_in = true;
                pthread_t new_receiver;
                receiver = new_receiver;
                pthread_create(&receiver, NULL, &receive, &sockfd);
            }
        }
        else if(is_str_equal(input, "/register ", 10)) {
            if (logged_in) {
                print_error("Already loggin in. Please log out first before registering...");
                continue;
            }
            memset(id, 0, MAX_NAME);
            int response = handle_register(input, id);
            if (response != -1) {
                sockfd = response;
                logged_in = true;
                pthread_t new_receiver;
                receiver = new_receiver;
                pthread_create(&receiver, NULL, &receive, &sockfd);
            }
        }
        else if (strcmp(input, "/logout") == 0) {
            handle_logout(sockfd, id);
            pthread_join(receiver, NULL);
            logged_in = false;
        }
        else if (is_str_equal(input, "/joinsession ", 13)) {
            handle_join_session(sockfd, input, id);
        }
        else if (strcmp(input, "/leavesession") == 0) {
            handle_leave_session(sockfd, id);
        }
        else if (is_str_equal(input, "/createsession ", 15)) {
            handle_create_session(sockfd, input, id);
        }
        else if (strcmp(input, "/list") == 0) {
            handle_list(sockfd, id);
            // read_from_stream(sockfd, buf);
            // int start = 0;
            // int colon_count = 0;
            // for (; colon_count < 3; start++) {
            //     if (buf[start] == ':') {
            //         colon_count++;
            //     }
            // }
            // for (; buf[start] != '\0'; start++) {
            //     fprintf(stdout, "%c", buf[start]);
            // }
        }
        else if (strcmp(input, "/quit") == 0) {
            if (logged_in) {
                handle_logout(sockfd, id);
                logged_in = false;
            }
            // close(sockfd);
            free(input);
            free(buf);
            free(id);
            pthread_join(receiver, NULL);
            fprintf(stdout, "Client terminated.\n");
            return 0;
        }
        else {
            // if (!joined_session) {
            //     fprintf(stdout, "Please join a session first.\n");
            //     continue;
            // }
            handle_send_msg(sockfd, input, id);
        }
    }
    return 0;
}