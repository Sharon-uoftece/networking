#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include "utils.h"
#include "server_utils.h"

#define backlog 16

user_t *known_users[MAX_USERS];

user_t *active_users[MAX_USERS];
pthread_mutex_t mutex_users;

session_t *active_sessions[MAX_SESSIONS];
pthread_mutex_t mutex_sessions;

int session_id = 0;

void print_active_users() {
    printf("Printing active users...\n");
    pthread_mutex_lock(&mutex_users);
    for (int i = 0; i < MAX_USERS; i++) {
        if (active_users[i] == NULL) {
            continue;
        }
        fprintf(stdout, "User %d has name %s and password %s\n", i, active_users[i]->client_name, active_users[i]->password);
    }
    pthread_mutex_unlock(&mutex_users);
}

void *user_handler(void *arg) {
    int sockfd = *(int *)arg;
    unsigned char *buf = malloc(MAX_MSG_STR);
    unsigned char *send_buf = malloc(MAX_MSG_STR);
    char source[] = "server";
    user_t *me = NULL;
    while (true) {
        memset(buf, 0, MAX_MSG_STR);
        memset(send_buf, 0, MAX_MSG_STR);
        int status = read_from_stream(sockfd, buf);
        if (status == 1) {
            fprintf(stdout, "Leaving thread...\n");
            // TODO: add code to clean up.
            free(buf);
            free(send_buf);
            close(sockfd);
            return NULL;
        }
        fprintf(stdout, "Received message: %s\n", buf);
        fprintf(stdout, "Message length: %d\n", strlen(buf));

        /* if the client is trying to login */
        if (is_str_equal(buf, "0:", 2)) {
            fprintf(stdout, "Checking login...\n");
            pthread_mutex_lock(&mutex_users);
            int response = handle_login(sockfd, buf, known_users, active_users);
            pthread_mutex_unlock(&mutex_users);

            char *msg = malloc(LOGIN_RESPONSE_LEN + 1);
            memset(msg, 0, LOGIN_RESPONSE_LEN + 1);
            if (response == USER_DNE) {
                fprintf(stdout, "Login failed, user not found\n");
                memcpy(msg, "100:31:server:Login failed -> user not found.", LOGIN_RESPONSE_LEN);
                send(sockfd, msg, LOGIN_RESPONSE_LEN, 0);
                close(sockfd);
                free(buf);
                free(send_buf);
                free(msg);
                return NULL;
            }
            else if (response == USER_ALREADY_LOGGED_IN) {
                fprintf(stdout, "Login failed, user already logged in\n");
                memcpy(msg, "101:31:server:Login failed -> user logged in.", LOGIN_RESPONSE_LEN);
                send(sockfd, msg, LOGIN_RESPONSE_LEN, 0);
                close(sockfd);
                free(buf);
                free(send_buf);
                free(msg);
                return NULL;
            }
            else if (response == INCORRECT_PASSWORD) {
                fprintf(stdout, "Login failed, incorrect password\n");
                memcpy(msg, "102:31:server:Login failed -> wrong password.", LOGIN_RESPONSE_LEN);
                send(sockfd, msg, LOGIN_RESPONSE_LEN, 0);
                close(sockfd);
                free(buf);
                free(send_buf);
                free(msg);
                return NULL;
            }
            me = active_users[response];
            fprintf(stdout, "Login successful\n");
            memcpy(msg, "1:31:server:Login was successful -> Have fun!", LOGIN_RESPONSE_LEN);
            pthread_mutex_lock(&(me->mutex_sockfd));
            send(sockfd, msg, LOGIN_RESPONSE_LEN, 0);
            pthread_mutex_unlock(&(me->mutex_sockfd));
            free(msg);
        }
        else if (is_str_equal(buf, "17:", 2)) {
            fprintf(stdout, "In process of trying register...\n");
            pthread_mutex_lock(&mutex_users);
            int response = handle_register(sockfd, buf, known_users, active_users);
            pthread_mutex_unlock(&mutex_users);

            char *msg = malloc(REGISTER_RESPONSE_LEN + 1);
            memset(msg, 0, REGISTER_RESPONSE_LEN + 1);
            if (response == REGISTER_SUCCESS) {
                fprintf(stdout, "Successfully register. You can try Login now...\n");
                memcpy(msg, "109:51:server: Register success -> you can try to log on now...", REGISTER_RESPONSE_LEN);
                send(sockfd, msg, REGISTER_RESPONSE_LEN, 0);
                close(sockfd);
                free(buf);
                free(send_buf);
                free(msg);
                return NULL;
            }
            else if (response == USERNAME_TAKEN) {
                fprintf(stdout, "Username already taken, please try with another name...\n");
                memcpy(msg, "107:51:server:Login failed -> username already in use.", REGISTER_RESPONSE_LEN);
                send(sockfd, msg, REGISTER_RESPONSE_LEN, 0);
                close(sockfd);
                free(buf);
                free(send_buf);
                free(msg);
                return NULL;
            }
            // else if (response == MAX_USER_EXCEEDED) {
            //     fprintf(stdout, "Cannot register new user, maximum user exceeded\n");
            //     memcpy(msg, "108:51:server: Register failed -> too many users", REGISTER_RESPONSE_LEN);
            //     send(sockfd, msg, REGISTER_RESPONSE_LEN, 0);
            //     close(sockfd);
            //     free(buf);
            //     free(send_buf);
            //     free(msg);
            //     return NULL;
            // }
            // me = active_users[response];
            // fprintf(stdout, "Login successful\n");
            // memcpy(msg, "1:31:server:Login was successful -> Have fun!", LOGIN_RESPONSE_LEN);
            // pthread_mutex_lock(&(me->mutex_sockfd));
            // send(sockfd, msg, LOGIN_RESPONSE_LEN, 0);
            // pthread_mutex_unlock(&(me->mutex_sockfd));
            free(msg);
        }
        /* if the client is trying to logout */
        else if (is_str_equal(buf, "3:", 2)) {
            fprintf(stdout, "Checking log out...\n");
            // check possible deadlock
            pthread_mutex_lock(&mutex_users);
            pthread_mutex_lock(&mutex_sessions);
            int response = handle_logout(buf, active_users, active_sessions);
            pthread_mutex_unlock(&mutex_users);
            pthread_mutex_unlock(&mutex_sessions);
            /* send response */
            if (response == 0) {
                char content[] = "Logout Successful.";
                message_t *msg = create_message(LOGOUT_ACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == -1) {
                char content[] = "Logout unsuccessful.";
                message_t *msg = create_message(LOGOUT_ACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            pthread_mutex_lock(&(me->mutex_sockfd));
            send(sockfd, send_buf, strlen(send_buf), 0);
            pthread_mutex_unlock(&(me->mutex_sockfd));
            close(sockfd);
            free(buf);
            free(send_buf);
            print_active_users();
            return NULL;
        }
        /* if the client is trying to join a session */
        else if (is_str_equal(buf, "4:", 2)) {
            fprintf(stdout, "Checking join session...\n");
            pthread_mutex_lock(&mutex_users);
            pthread_mutex_lock(&mutex_sessions);
            int response = handle_join_session(buf, active_users, active_sessions);
            pthread_mutex_unlock(&mutex_users);
            pthread_mutex_unlock(&mutex_sessions);
            if (response == 0) {
                char content[] = "Join session successful.";
                message_t *msg = create_message(JOIN_ACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == USER_NOT_FOUND) {
                char content[] = "Join session unsuccessful. User not found. Please log in first.";
                message_t *msg = create_message(JOIN_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == USER_ALREADY_IN_SESSION) {
                char content[] = "Join session unsuccessful. You are only allowed to join one session at a time, please leave your current session first.";
                message_t *msg = create_message(JOIN_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == SESSION_NOT_FOUND) {
                char content[] = "Join session unsuccessful. Session to join does not exist. Please create first.";
                message_t *msg = create_message(JOIN_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            pthread_mutex_lock(&(me->mutex_sockfd));
            send(sockfd, send_buf, strlen(send_buf), 0);
            pthread_mutex_unlock(&(me->mutex_sockfd));
        }
        /* if the client is trying to leave a session */
        else if (is_str_equal(buf, "7:", 2)) {
            fprintf(stdout, "Checking leave session...\n");
            pthread_mutex_lock(&mutex_users);
            pthread_mutex_lock(&mutex_sessions);
            int response = handle_leave_session(buf, active_users, active_sessions);
            pthread_mutex_unlock(&mutex_users);
            pthread_mutex_unlock(&mutex_sessions);
            if (response == 0) {
                char content[] = "Leave session successful.";
                message_t *msg = create_message(LEAVE_ACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == USER_NOT_FOUND) {
                char content[] = "Leave session unsuccessful. Please join a session first.";
                message_t *msg = create_message(LEAVE_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            pthread_mutex_lock(&(me->mutex_sockfd));
            send(sockfd, send_buf, strlen(send_buf), 0);
            pthread_mutex_unlock(&(me->mutex_sockfd));
        }
        /* if the client is trying to create a session */
        else if (is_str_equal(buf, "8:", 2)) {
            fprintf(stdout, "Checking create session...\n");
            pthread_mutex_lock(&mutex_users);
            pthread_mutex_lock(&mutex_sessions);
            int response = handle_create_sesssion(buf, active_users, active_sessions);
            pthread_mutex_unlock(&mutex_users);
            pthread_mutex_unlock(&mutex_sessions);
            if (response == 0) {
                char content[] = "Create session successful.";
                message_t *msg = create_message(NEW_SESSION_ACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == USER_NOT_FOUND) {
                char content[] = "Create session unsuccessful. Please log in first.";
                message_t *msg = create_message(NEW_SESSION_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == USER_ALREADY_IN_SESSION) {
                char content[] = "Create session unsuccessful. You are only allowed to join one session at a time, please leave your current session first.";
                message_t *msg = create_message(NEW_SESSION_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            else if (response == SESSION_ID_EXISTS) {
                char content[] = "Create session unsuccessful. Session to be created already exists.";
                message_t *msg = create_message(NEW_SESSION_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
            }
            pthread_mutex_lock(&(me->mutex_sockfd));
            send(sockfd, send_buf, strlen(send_buf), 0);
            pthread_mutex_unlock(&(me->mutex_sockfd));
        }
        /* if the client is trying to send a message */
        else if (is_str_equal(buf, "10:", 3)) {
            fprintf(stdout, "Checking message...");
            pthread_mutex_lock(&mutex_users);
            pthread_mutex_lock(&mutex_sessions);
            int response = handle_message(buf, active_users, active_sessions);
            pthread_mutex_unlock(&mutex_users);
            pthread_mutex_unlock(&mutex_sessions);
            if (response == USER_NOT_FOUND) {
                char content[] = "Couldn't send messsage. Please join a session first.";
                message_t *msg = create_message(NEW_SESSION_NACK, strlen(content), source, content);
                char *msg_str = msg_to_str(msg);
                memcpy(send_buf, msg_str, strlen(msg_str));
                free(msg);
                free(msg_str);
                pthread_mutex_lock(&(me->mutex_sockfd));
                send(sockfd, send_buf, strlen(send_buf), 0);
                pthread_mutex_unlock(&(me->mutex_sockfd));
            }
        }
        /* if the user is trying to list */
        else if (is_str_equal(buf, "11:", 3)) {
            fprintf(stdout, "Checking query...\n");
            pthread_mutex_lock(&mutex_users);
            pthread_mutex_lock(&mutex_sessions);
            char *list = handle_query(known_users, active_users, active_sessions);
            pthread_mutex_unlock(&mutex_users);
            pthread_mutex_unlock(&mutex_sessions);
            fprintf(stdout, "%s", list);
            message_t *msg = create_message(QUERY_ACK, strlen(list), source, list);
            char *msg_str = msg_to_str(msg);
            pthread_mutex_lock(&(me->mutex_sockfd));
            send(sockfd, msg_str, strlen(msg_str), 0);
            pthread_mutex_unlock(&(me->mutex_sockfd));
            free(msg);
            free(msg_str);
            free(list);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2 || !is_number_str(argv[1])) {
        print_usage();
    }

    for (int i = 0; i < MAX_USERS; i++) {
        active_users[i] = NULL;
        user_t *user = malloc(sizeof(user_t));
        memset(user, 0, sizeof(user_t));
        char password[] = " ";
        memcpy(user->password, password, 1);
        user->id = -1;
        memset(user->client_name, 0, MAX_NAME);
        user->sockfd = -1;
        user->session_id = -1;
        known_users[i] = user;
    }

    // for (int i = 0; i < MAX_USERS; i++) {
    //     active_users[i] = NULL;
    //     user_t *user = malloc(sizeof(user_t));
    //     memset(user, 0, sizeof(user_t));
    //     char password[] = "abcd";
    //     memcpy(user->password, password, 4);
    //     user->id = i;
    //     user->client_name[0] = 'a' + i;
    //     user->sockfd = -1;
    //     user->session_id = -1;
    //     known_users[i] = user;
    // }

    // for (int i = 0; i < MAX_USERS; i++) {
    //     active_users[i] = NULL;
    //     user_t *user = malloc(sizeof(user_t));
    //     memset(user, 0, sizeof(user_t));
    //     known_users[i] = user;
    // }

    for (int i = 0; i < MAX_SESSIONS; i++) {
        active_sessions[i] = NULL;
    }

    pthread_mutex_init(&mutex_users, NULL);
    pthread_mutex_init(&mutex_sessions, NULL);
    struct addrinfo hints;
    struct addrinfo *myinfo;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int listener;
    int sockfd;
    int status;
    char *hostname = malloc(256);
    memset(hostname, 0, 256);
    memset(&hints, 0, sizeof(hints));
    status = gethostname(hostname, 256);
    if (status) {
        free(hostname);
        print_error("gethostname() failed on server.");
        return 1;
    }
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        free(hostname);
        print_error("socket creation failed on server.");
        return 1;
    }
    status = getaddrinfo(hostname, argv[1], &hints, &myinfo);
    if (status == -1) {
        freeaddrinfo(myinfo);
        free(hostname);
        print_error("getaddrinfo() failed on server.");
        return 1;
    }
    status = bind(listener, myinfo->ai_addr, myinfo->ai_addrlen);
    if (status == -1) {
        freeaddrinfo(myinfo);
        free(hostname);
        print_error("bind failed on server.");
        return 1;
    }
    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        fprintf(stdout, "Listening...\n");
        status = listen(listener, backlog);
        if (status == -1) {
            print_error("listen failed on server.");
            freeaddrinfo(myinfo);
            free(hostname);
            return 1;
        }
        int *sockfd = malloc(sizeof(int));
        *sockfd = accept(listener, (struct sockaddr *) &client_addr, &client_addr_len);
        fprintf(stdout, "Created new sockfd\n");
        if (*sockfd == -1) {
            print_error("listen failed on server.");
            freeaddrinfo(myinfo);
            free(hostname);
            return 1;
        }
        pthread_t new_thread;
        pthread_create(&new_thread, NULL, &user_handler, sockfd);
    }
    freeaddrinfo(myinfo);
    free(hostname);
    return 0;
}
