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
#include "server_utils.h"

message_t *create_message(unsigned type, unsigned size, unsigned char *source, unsigned char *data) {
    message_t *msg = malloc(sizeof(message_t));
    memset(msg, 0, sizeof(message_t));
    msg->type = type;
    msg->size = size;
    memcpy(msg->source, source, strlen(source));
    memcpy(msg->data, data, size);
    return msg;
}

unsigned char *msg_to_str(message_t* msg) {
    unsigned char *msg_str = malloc(MAX_MSG_STR);
    memset(msg_str, 0, MAX_MSG_STR);
    unsigned char *type_str = malloc(sizeof(char) * 3);
    unsigned char *size_str = malloc(sizeof(char) * 16);
    memset(type_str, 0, 3);
    memset(size_str, 0, 16);
    sprintf(type_str, "%u", msg->type);
    sprintf(size_str, "%u", msg->size);
    int msg_str_pointer = 0;
    for (int i = 0; i < strlen(type_str); i++) {
        msg_str[msg_str_pointer++] = type_str[i];
    }
    msg_str[msg_str_pointer++] = ':';
    for (int i = 0; i < strlen(size_str); i++) {
        msg_str[msg_str_pointer++] = size_str[i];
    }
    msg_str[msg_str_pointer++] = ':';
    for (int i = 0; i < strlen(msg->source); i++) {
        msg_str[msg_str_pointer++] = msg->source[i];
    }
    msg_str[msg_str_pointer++] = ':';
    for (int i = 0; i < msg->size; i++) {
        msg_str[msg_str_pointer++] = msg->data[i];
    }
    free(type_str);
    free(size_str);
    return msg_str;
}

/* server utils */
int is_str_equal(unsigned char *s1, unsigned char *s2, int length) {
    for (int i = 0; i < length; i++) {
        if (s1[i] == '\0' || s2[i] == '\0') {
            return 0;
        }
        if (s1[i] != s2[i]) {
            return 0;
        }
    }
    return 1;
}

// check if string is a base 10 number
int is_number_str(unsigned char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

void print_error(unsigned char *str) {
    fprintf(stdout, "%s", red);
    fprintf(stdout, "%s\n", str);
    fprintf(stdout, "%s", black);
}

void print_usage() {
    print_error("usage: server tcp_port_number");
    exit(0);
}

// respect messsage boundaries
int read_from_stream(int sockfd, unsigned char *buf) {
    memset(buf, 0, MAX_MSG_STR);
    unsigned char *slot = malloc(1);
    unsigned char *size_str = malloc(16);
    memset(size_str, 0, 16);
    int buf_ptr = 0;
    int size_ptr = 0;
    long int size = 0;
    int colon_count = 0;
    while (colon_count < 3) {
        // receive chars one by one
        int bytes = recv(sockfd, slot, 1, 0);
        if (bytes == 0) {
            fprintf(stdout, "Socket closed on the other side.\n");
            return 1;
        }
        // printf("Bytes: %d\n", bytes);
        // if the char is : then increment colon count
        if (slot[0] == ':') {
            colon_count++;
            buf[buf_ptr++] = slot[0];
            continue;
        }
        // if we've seen 1 semicolon, then start recording the size
        if (colon_count == 1) {
            size_str[size_ptr++] = slot[0];
        }
        buf[buf_ptr++] = slot[0];
    }
    char *dummy;
    size = strtol(size_str, &dummy, 10);
    for (int i = 0; i < size; i++) {
        int bytes = recv(sockfd, slot, 1, 0);
        if (bytes == 0) {
            fprintf(stdout, "Socket closed on the other side.\n");
            return 1;
        }
        // printf("Bytes: %d\n", bytes);
        buf[buf_ptr++] = slot[0];
    }
    free(slot);
    free(size_str);
    return 0;
}

int handle_login(int sockfd, unsigned char *msg, user_t *known_users[], user_t *active_users[]) {
    /* parse data */
    unsigned char *client_id = malloc(MAX_NAME);
    unsigned char *password = malloc(MAX_NAME);
    unsigned char *size_str = malloc(16);
    memset(client_id, 0, MAX_NAME);
    memset(password, 0, MAX_NAME);
    memset(size_str, 0, 16);

    /* extract size */
    int msg_pointer = 2;
    int i = 0;
    while (msg[msg_pointer] != ':') {
        size_str[i++] = msg[msg_pointer++];
    }
    char *dummy;
    long int size = strtol(size_str, &dummy, 10);
    /* extract client id */
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }
    /* extract password */
    msg_pointer++;
    for (i = 0; i < size; i++) {
        password[i] = msg[msg_pointer++];
    }
    for (i = 0; i < MAX_USERS; i++) {
        if (strcmp(known_users[i]->client_name, client_id) == 0 && strcmp(known_users[i]->password, password) == 0) {
            /* check if the user is already active (is already logged in)*/
            for (int j = 0; j < MAX_USERS; j++) {
                if (active_users[j] != NULL && strcmp(active_users[j]->client_name, client_id) == 0) {
                    free(client_id);
                    free(password);
                    free(size_str);
                    return USER_ALREADY_LOGGED_IN;
                }
            }
            /* add user to active users */
            for (int j = 0; j < MAX_USERS; j++) {
                if (active_users[j] == NULL) {
                    user_t *user = malloc(sizeof(user_t));
                    memmove(user, known_users[i], sizeof(user_t));
                    user->sockfd = sockfd;
                    pthread_mutex_t mutex;
                    user->mutex_sockfd = mutex;
                    pthread_mutex_init(&user->mutex_sockfd, NULL);
                    active_users[j] = user;
                    free(client_id);
                    free(password);
                    free(size_str);
                    return j;
                }
            }
        }
        /* if user exists but password doesn't match */
        else if (strcmp(known_users[i]->client_name, client_id) == 0 && strcmp(known_users[i]->password, password) != 0) {
            free(client_id);
            free(password);
            free(size_str);
            return INCORRECT_PASSWORD;
        }
    }
    free(client_id);
    free(password);
    free(size_str);
    return USER_DNE;
}

int handle_register(int sockfd, unsigned char *msg, user_t *known_users[], user_t *active_users[]) {
    /* parse data */
    unsigned char *client_id = malloc(MAX_NAME);
    unsigned char *password = malloc(MAX_NAME);
    unsigned char *size_str = malloc(16);
    memset(client_id, 0, MAX_NAME);
    memset(password, 0, MAX_NAME);
    memset(size_str, 0, 16);

    /* extract size */
    int msg_pointer = 2;
    int i = 0;
    while (msg[msg_pointer] != ':') {
        size_str[i++] = msg[msg_pointer++];
    }
    char *dummy;
    long int size = strtol(size_str, &dummy, 10);
    /* extract client id */
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }
    /* extract password */
    msg_pointer++;
    int lenPassword = 0;
    for (i = 0; i < size; i++) {
        password[i] = msg[msg_pointer++];
        lenPassword++;
    }

    int freeIndex = -1;
    int foundFree = -1;

    for (i = 0; i < MAX_USERS; i++) {
        //known_users: existed users
        //user->client_name[0] = 'initialyInitializedToNULL';

        if (foundFree == -1 && known_users[i]->id == -1) {
            freeIndex = i;
            foundFree = 0;
        }
        if (strcmp(known_users[i]->client_name, client_id) == 0) {
            free(client_id);
            free(password);
            free(size_str);
            return USERNAME_TAKEN;
        }
    }

    // if (foundFree == -1) {
    //     free(client_id);
    //     free(password);
    //     free(size_str);
    //     return MAX_USER_EXCEEDED;
    // }

    user_t *user = malloc(sizeof(user_t));
    memset(user, 0, sizeof(user_t));
    // char password[] = password;
    memcpy(user->password, password, lenPassword);
    user->id = freeIndex;
    memcpy(user->client_name, client_id, strlen(client_id));
    user->sockfd = -1;
    user->session_id = -1;
    known_users[freeIndex] = user;
        
    return REGISTER_SUCCESS;
    // free(client_id);
    // free(password);
    // free(size_str);
    // return USER_DNE;
}

int handle_logout(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]) {
    unsigned char *client_id = malloc(MAX_NAME);
    memset(client_id, 0, MAX_NAME);
    int msg_pointer = 4;
    int i = 0;
    int user_id = -100;
    /* get client name from message */
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }
    /* find and free user from active users*/
    for (int i = 0; i < MAX_USERS; i++) {
        if (active_users[i] != NULL && strcmp(active_users[i]->client_name, client_id) == 0) {
            user_id = active_users[i]->id;
            pthread_mutex_destroy(&active_users[i]->mutex_sockfd);
            free(active_users[i]);
            active_users[i] = NULL;
        }
    }

    if (user_id == -100) {
        return -1;
    }

    /* find and remove user from his/her session */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (active_sessions[i] == NULL) {
            continue;
        }
        for (int j = 0; j < MAX_SESSION_USERS; j++) {
            if (active_sessions[i]->users[j] == user_id) {
                active_sessions[i]->users[j] = -1;
                active_sessions[i]->num_users--;
                /* free the session if it has no more users */
                if (active_sessions[i]->num_users == 0) {
                    free(active_sessions[i]);
                    active_sessions[i] = NULL;
                }
                // return as user can only be in 1 session
                free(client_id);
                return 0;
            }
        }
    }
    free(client_id);
    return 0;
}

int handle_join_session(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]) {
    unsigned char *size_str = malloc(16);
    unsigned char *client_id = malloc(MAX_NAME);
    unsigned char *session_name = malloc(MAX_NAME);
    memset(size_str, 0, 16);
    memset(client_id, 0, MAX_NAME);
    memset(session_name, 0, MAX_NAME);
    int msg_pointer = 2;
    int i = 0;
    user_t *user = NULL;
    session_t *session = NULL;
    /* get size */
    while (msg[msg_pointer] != ':') {
        size_str[i++] = msg[msg_pointer++];
    }
    char *dummy;
    long int size = strtol(size_str, &dummy, 10);

    /* get client id */
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }

    msg_pointer++;
    for (i = 0; i < size; i++) {
        session_name[i] = msg[msg_pointer++];
    }

    /* check if the user is logged in */
    for (i = 0; i < MAX_USERS; i++) {
        if (active_users[i] != NULL && strcmp(active_users[i]->client_name, client_id) == 0) {
            user = active_users[i];
            break;
        }
    }

    if (user == NULL) {
        free(size_str);
        free(client_id);
        free(session_name);
        return USER_NOT_FOUND;
    }

    // user can only be in 1 session
    if (user->session_id != -1) {
        free(size_str);
        free(client_id);
        free(session_name);
        return USER_ALREADY_IN_SESSION;
    }
    
    /* check if the session to be joined exists */
    for (i = 0; i < MAX_SESSIONS; i++) {
        if (active_sessions[i] != NULL && strcmp(active_sessions[i]->name, session_name) == 0) {
            session = active_sessions[i];
            break;
        }
    }
    
    if (session == NULL) {
        free(size_str);
        free(client_id);
        free(session_name);
        return SESSION_NOT_FOUND;
    }

    // add user id to user list of session
    for (i = 0; i < MAX_SESSION_USERS; i++) {
        if (session->users[i] == -1) {
            user->session_id = session->id;
            session->users[i] = user->id;
            session->num_users++;
            break;
        }
    }

    free(size_str);
    free(client_id);
    free(session_name);
    return 0;
}

int handle_create_sesssion(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]) {
    unsigned char *size_str = malloc(16);
    unsigned char *client_id = malloc(MAX_NAME);
    unsigned char *session_name = malloc(MAX_NAME);
    memset(size_str, 0, 16);
    memset(client_id, 0, MAX_NAME);
    memset(session_name, 0, MAX_NAME);
    int msg_pointer = 2;
    int i = 0;
    user_t *user = NULL;
    /* get size */
    while (msg[msg_pointer] != ':') {
        size_str[i++] = msg[msg_pointer++];
    }
    char *dummy;
    long int size = strtol(size_str, &dummy, 10);

    /* get client id */
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }

    msg_pointer++;
    for (i = 0; i < size; i++) {
        session_name[i] = msg[msg_pointer++];
    }

    /* check if the user is logged in */
    for (i = 0; i < MAX_USERS; i++) {
        if (active_users[i] != NULL && strcmp(active_users[i]->client_name, client_id) == 0) {
            user = active_users[i];
            break;
        }
    }

    if (user == NULL) {
        free(size_str);
        free(client_id);
        free(session_name);
        return USER_NOT_FOUND;
    }

    // user can only be in 1 session
    if (user->session_id != -1) {
        free(size_str);
        free(client_id);
        free(session_name);
        return USER_ALREADY_IN_SESSION;
    }
    
    /* check if the session to be created exists */
    for (i = 0; i < MAX_SESSIONS; i++) {
        if (active_sessions[i] != NULL && strcmp(active_sessions[i]->name, session_name) == 0) {
            free(size_str);
            free(client_id);
            free(session_name);
            return SESSION_ID_EXISTS;
        }
    }

    for (i = 0; i < MAX_SESSIONS; i++) {
        if (active_sessions[i] == NULL) {
            session_t *new_session = malloc(sizeof(session_t));
            memset(new_session, 0, sizeof(session_t));
            new_session->id = i;
            for (int j = 0; j < MAX_SESSION_USERS; j++) {
                new_session->users[j] = -1;
            }
            new_session->users[0] = user->id;
            new_session->num_users = 1;
            user->session_id = i;
            memcpy(new_session->name, session_name, strlen(session_name));
            active_sessions[i] = new_session;
            break;
        }
    }
    free(size_str);
    free(client_id);
    free(session_name);
    return 0;
}

int handle_leave_session(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]) {
    unsigned char *size_str = malloc(16);
    unsigned char *client_id = malloc(MAX_NAME);
    unsigned char *session_name = malloc(MAX_NAME);
    memset(size_str, 0, 16);
    memset(client_id, 0, MAX_NAME);
    memset(session_name, 0, MAX_NAME);
    int msg_pointer = 2;
    int i = 0;
    user_t *user = NULL;
    session_t *session = NULL;
    /* get size */
    while (msg[msg_pointer] != ':') {
        size_str[i++] = msg[msg_pointer++];
    }
    char *dummy;
    long int size = strtol(size_str, &dummy, 10);

    /* get client id */
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }

    msg_pointer++;
    for (i = 0; i < size; i++) {
        session_name[i] = msg[msg_pointer++];
    }

    /* check if the user is logged in */
    for (i = 0; i < MAX_USERS; i++) {
        if (active_users[i] != NULL && strcmp(active_users[i]->client_name, client_id) == 0) {
            user = active_users[i];
            break;
        }
    }

    if (user == NULL || user->session_id == -1) {
        free(size_str);
        free(client_id);
        free(session_name);
        return USER_NOT_FOUND;
    }

    int session_index = user->session_id;
    session = active_sessions[session_index];

    for (i = 0; i < MAX_SESSION_USERS; i++) {
        if (session->users[i] == user->id) {
            session->users[i] = -1;
            session->num_users--;
            user->session_id = -1;
            if (session->num_users == 0) {
                free(active_sessions[session_index]);
                active_sessions[session_index] = NULL;
            }
            break;
        }
    }
    free(size_str);
    free(client_id);
    free(session_name);
    return 0;
}

int handle_message(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]) {
    unsigned char *size_str = malloc(16);
    unsigned char *client_id = malloc(MAX_NAME);
    unsigned char *text = malloc(MAX_DATA);
    memset(size_str, 0, 16);
    memset(client_id, 0, MAX_NAME);
    memset(text, 0, MAX_DATA);
    int msg_pointer = 3;
    int i = 0;
    user_t *user = NULL;
    session_t *session = NULL;
    /* get size */
    while (msg[msg_pointer] != ':') {
        size_str[i++] = msg[msg_pointer++];
    }
    char *dummy;
    long int size = strtol(size_str, &dummy, 10);

    /* get client id */
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ':') {
        client_id[i++] = msg[msg_pointer++];
    }

    /* get text */
    msg_pointer++;
    for (i = 0; i < size; i++) {
        text[i] = msg[msg_pointer++];
    }

    /* check if the user is logged in */
    for (i = 0; i < MAX_USERS; i++) {
        if (active_users[i] != NULL && strcmp(active_users[i]->client_name, client_id) == 0) {
            user = active_users[i];
            break;
        }
    }

    if (user == NULL || user->session_id == -1) {
        free(size_str);
        free(client_id);
        free(text);
        return USER_NOT_FOUND;
    }

    session = active_sessions[user->session_id];
    for (int i = 0; i < MAX_SESSION_USERS; i++) {
        if (session->users[i] == -1 || session->users[i] == user->id) {
            continue;
        }
        int user_id = session->users[i];
        for (int j = 0; j < MAX_USERS; j++) {
            if (active_users[j] != NULL && active_users[j]->id == user_id) {
                int sockfd = active_users[j]->sockfd;
                pthread_mutex_lock(&active_users[j]->mutex_sockfd);
                send(sockfd, msg, strlen(msg), 0);
                fprintf(stdout, "Sent message to user: %s\n", active_users[j]->client_name);
                pthread_mutex_unlock(&active_users[j]->mutex_sockfd);
            }
        }
    }
    free(size_str);
    free(client_id);
    free(text);
    return 0;
}

unsigned char *handle_query(user_t *known_users[], user_t *active_users[], session_t *active_sessions[]) {
    unsigned int list_size = MAX_USERS * MAX_NAME + MAX_SESSIONS * MAX_NAME + MAX_SESSIONS * MAX_SESSION_USERS * MAX_NAME + 1024;
    unsigned char *list = malloc(list_size);
    memset(list, 0, list_size);
    unsigned char divider[] = "----------------------------------\n";
    unsigned char user[] = "User: ";
    unsigned char session[] = "Session: ";
    unsigned char users[] = "Current Active Users:\n";
    unsigned char sessions[] = "Current Active Sessions:\n";
    unsigned char session_users[] = "Session Has Active Users: ";
    int list_ptr = 0;
    /* print users */
    for (int i = 0; i < strlen(divider); i++) {
        list[list_ptr++] = divider[i];
    }
    for (int i = 0; i < strlen(users); i++) {
        list[list_ptr++] = users[i];
    }
    for (int i = 0; i < MAX_USERS; i++) {
        if (active_users[i] == NULL) {
            continue;
        }
        unsigned char *name = active_users[i]->client_name;
        for (int j = 0; j < strlen(user); j++) {
            list[list_ptr++] = user[j];
        }
        for (int j = 0; j < strlen(name); j++) {
            list[list_ptr++] = name[j];
        }
        list[list_ptr++] = '\n';
    }

    /* print sessions */
    for (int i = 0; i < strlen(divider); i++) {
        list[list_ptr++] = divider[i];
    }
    for (int i = 0; i < strlen(sessions); i++) {
        list[list_ptr++] = sessions[i];
    }
    
    for (int i = 0; i < MAX_USERS; i++) {
        if (active_sessions[i] == NULL) {
            continue;
        }
        unsigned char *name = active_sessions[i]->name;
        for (int j = 0; j < strlen(session); j++) {
            list[list_ptr++] = session[j];
        }
        for (int j = 0; j < strlen(name); j++) {
            list[list_ptr++] = name[j];
        }
        list[list_ptr++] = '\n';
        list[list_ptr++] = '\t';

        /* print session users */
        for (int j = 0; j < strlen(session_users); j++) {
            list[list_ptr++] = session_users[j];
        }
        for (int j = 0; j < MAX_SESSION_USERS; j++) {
            if (active_sessions[i]->users[j] == -1) {
                continue;
            }
            int user_id = active_sessions[i]->users[j];
            list[list_ptr++] = '\n';
            list[list_ptr++] = '\t';
            list[list_ptr++] = '\t';
            for (int k = 0; k < strlen(known_users[user_id]->client_name); k++) {
                list[list_ptr++] = known_users[user_id]->client_name[k];
            }
        }
        list[list_ptr++] = '\n';
    }

    for (int i = 0; i < strlen(divider); i++) {
        list[list_ptr++] = divider[i];
    }

    return list;
}

