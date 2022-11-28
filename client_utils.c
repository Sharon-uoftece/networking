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

/* client utils */
void print_blue_arrow() {
    fprintf(stdout, bold_green "--> " black bold_off);
}

void print_bold_arrow() {
    fprintf(stdout, bold "--> " bold_off);
}

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

void free_message(message_t *msg) {
    free(msg);
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

/***********
 * /login 
 * <client ID>
 * <password> 
 * <server-IP>
 * <server-port>
 * Log into the server at the given address and port. 
 * The IP address is specified in the dotted decimal format.
 * returns -1 on error, sockfd on success.
***********/
int handle_login(unsigned char *msg, unsigned char *client_id) {
    /* parse data */
    unsigned char *password = malloc(MAX_NAME);
    unsigned char *serverIP = malloc(16);
    unsigned char *server_port = malloc(6);
    memset(client_id, 0, MAX_NAME);
    memset(password, 0, MAX_NAME);
    memset(serverIP, 0, 16);
    memset(server_port, 0, 6);
    int msg_pointer = 7;
    int i = 0;

    // extract client id
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        client_id[i++] = msg[msg_pointer++];
    }

    if (msg[msg_pointer] == '\0') {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    // extract password
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        password[i++] = msg[msg_pointer++];
    }
    if (msg[msg_pointer] == '\0') {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    // extract server ip
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        serverIP[i++] = msg[msg_pointer++];
    }
    if (msg[msg_pointer] == '\0') {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    // extract server port
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        server_port[i++] = msg[msg_pointer++];
    }

    if (!is_number_str(server_port)) {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    /* try to connect to server and send message */
    struct addrinfo hints;
    struct addrinfo *host_addr;
    int port = 8000;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int status = getaddrinfo(serverIP, server_port, &hints, &host_addr);
    if (status == -1) {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }
    message_t *message_wrapper = create_message(LOGIN, strlen(password), client_id, password);
    unsigned char *msg_str = msg_to_str(message_wrapper);
    // fprintf(stdout, "Parsed message to: %s\n", msg_str);
    // fprintf(stdout, "HOST_ADDR: 0x%x\n", host_addr->ai_addr);
    status = connect(sockfd, host_addr->ai_addr, host_addr->ai_addrlen);
    if (status == -1) {
        print_error("Failed to send message for login.");
        free(message_wrapper);
        free(msg_str);
        free(password);
        free(serverIP);
        free(server_port);
        freeaddrinfo(host_addr);
        return -1;
    }
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for login.");
        free(message_wrapper);
        free(msg_str);
        free(password);
        free(serverIP);
        free(server_port);
        close(sockfd);
        freeaddrinfo(host_addr);
        return -1;
    }
    memset(msg_str, 0, MAX_MSG_STR);
    bytes = recv(sockfd, msg_str, LOGIN_RESPONSE_LEN, 0);
    if (!is_str_equal(msg_str, "1:", 2)) {
        if (is_str_equal(msg_str, "100:", 4)) {
            print_bold_arrow();
            fprintf(stdout, "Server message: ");
            fprintf(stdout, "Login unsuccessful. User not recognized.\n");
        }
        else if (is_str_equal(msg_str, "101:", 4)) {
            print_bold_arrow();
            fprintf(stdout, "Server message: ");
            fprintf(stdout, "Login unsuccessful. User already logged in.\n");
        }
        else if (is_str_equal(msg_str, "102:", 4)) {
            print_bold_arrow();
            fprintf(stdout, "Server message: ");
            fprintf(stdout, "Login unsuccessful. Invalid password.\n");
        }
        free(message_wrapper);
        free(msg_str);
        free(password);
        free(serverIP);
        free(server_port);
        close(sockfd);
        freeaddrinfo(host_addr);
        return -1;
    }
    print_bold_arrow();
    fprintf(stdout, "Server message: ");
    fprintf(stdout, "Login successful!\n");
    free(message_wrapper);
    free(msg_str);
    free(password);
    free(serverIP);
    free(server_port);
    freeaddrinfo(host_addr);
    return sockfd;

}

int handle_register(unsigned char *msg, unsigned char *client_id) {
    /* parse data */
    unsigned char *password = malloc(MAX_NAME);
    unsigned char *serverIP = malloc(16);
    unsigned char *server_port = malloc(6);
    memset(client_id, 0, MAX_NAME);
    memset(password, 0, MAX_NAME);
    memset(serverIP, 0, 16);
    memset(server_port, 0, 6);
    int msg_pointer = 10;
    int i = 0;

    // extract client id
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        client_id[i++] = msg[msg_pointer++];
    }

    if (msg[msg_pointer] == '\0') {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    // extract password
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        password[i++] = msg[msg_pointer++];
    }
    if (msg[msg_pointer] == '\0') {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    // extract server ip
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        serverIP[i++] = msg[msg_pointer++];
    }
    if (msg[msg_pointer] == '\0') {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    // extract server port
    msg_pointer++;
    i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        server_port[i++] = msg[msg_pointer++];
    }

    if (!is_number_str(server_port)) {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }

    /* try to connect to server and send message */
    struct addrinfo hints;
    struct addrinfo *host_addr;
    int port = 8000;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int status = getaddrinfo(serverIP, server_port, &hints, &host_addr);
    if (status == -1) {
        free(password);
        free(serverIP);
        free(server_port);
        return -1;
    }
    message_t *message_wrapper = create_message(REGISTER, strlen(password), client_id, password);
    unsigned char *msg_str = msg_to_str(message_wrapper);
    // fprintf(stdout, "Parsed message to: %s\n", msg_str);
    // fprintf(stdout, "HOST_ADDR: 0x%x\n", host_addr->ai_addr);
    status = connect(sockfd, host_addr->ai_addr, host_addr->ai_addrlen);
    if (status == -1) {
        print_error("Failed to send message for register.");
        free(message_wrapper);
        free(msg_str);
        free(password);
        free(serverIP);
        free(server_port);
        freeaddrinfo(host_addr);
        return -1;
    }
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for register.");
        free(message_wrapper);
        free(msg_str);
        free(password);
        free(serverIP);
        free(server_port);
        close(sockfd);
        freeaddrinfo(host_addr);
        return -1;
    }
    memset(msg_str, 0, MAX_MSG_STR);
    bytes = recv(sockfd, msg_str, REGISTER_RESPONSE_LEN, 0);
    if (!is_str_equal(msg_str, "1:", 2)) {
        if (is_str_equal(msg_str, "107:", 4)) {
            print_bold_arrow();
            fprintf(stdout, "Server message: ");
            fprintf(stdout, "Register unsuccessful. Username already taken.\n");
        }
        else if (is_str_equal(msg_str, "108:", 4)) {
            print_bold_arrow();
            fprintf(stdout, "Server message: ");
            fprintf(stdout, "esRegister unsuccessful. MAX user achieved.\n");
        }
        free(message_wrapper);
        free(msg_str);
        free(password);
        free(serverIP);
        free(server_port);
        close(sockfd);
        freeaddrinfo(host_addr);
        return -1;
    }
    print_bold_arrow();
    fprintf(stdout, "Server message: ");
    fprintf(stdout, "Register successful!\n");
    free(message_wrapper);
    free(msg_str);
    free(password);
    free(serverIP);
    free(server_port);
    freeaddrinfo(host_addr);
    return sockfd;

}

int handle_logout(int sockfd, unsigned char *client_id) {
    // send message to server to log out
    message_t *message_wrapper = create_message(EXIT, 0, client_id, "");
    unsigned char *msg_str = msg_to_str(message_wrapper);
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for logout.");
        free(message_wrapper);
        free(msg_str);
        return -1;
    }
    free(message_wrapper);
    free(msg_str);
    return 0;
}

int handle_join_session(int sockfd, unsigned char *msg, unsigned char *client_id) {
    unsigned char *session_id = malloc(MAX_NAME);
    memset(session_id, 0, MAX_NAME);
    int msg_pointer = 13;
    int i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        session_id[i++] = msg[msg_pointer++];
    }
    message_t *message_wrapper = create_message(JOIN_SESSION, strlen(session_id), client_id, session_id);
    unsigned char *msg_str = msg_to_str(message_wrapper);
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for join session.");
        free(message_wrapper);
        free(msg_str);
        free(session_id);
        return -1;
    }
    free(message_wrapper);
    free(msg_str);
    free(session_id);
    return 0;
}

int handle_leave_session(int sockfd, unsigned char *client_id) {
    message_t *message_wrapper = create_message(LEAVE_SESSION, 0, client_id, "");
    unsigned char *msg_str = msg_to_str(message_wrapper);
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for leave session.");
        free(message_wrapper);
        free(msg_str);
        return -1;
    }
    free(message_wrapper);
    free(msg_str);
    return 0;
}

int handle_create_session(int sockfd, unsigned char *msg, unsigned char *client_id) {
    unsigned char *session_id = malloc(MAX_NAME);
    memset(session_id, 0, MAX_NAME);
    int msg_pointer = 15;
    int i = 0;
    while (msg[msg_pointer] != ' ' && msg[msg_pointer] != '\n' && msg[msg_pointer] != '\0') {
        session_id[i++] = msg[msg_pointer++];
    }
    message_t *message_wrapper = create_message(NEW_SESSION, strlen(session_id), client_id, session_id);
    unsigned char *msg_str = msg_to_str(message_wrapper);
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for create session.");
        free(message_wrapper);
        free(msg_str);
        free(session_id);
        return -1;
    }
    free(message_wrapper);
    free(msg_str);
    free(session_id);
    return 0;
}

int handle_list(int sockfd, unsigned char *client_id) {
    message_t *message_wrapper = create_message(QUERY, 0, client_id, "");
    unsigned char *msg_str = msg_to_str(message_wrapper);
    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for list.");
        free(message_wrapper);
        free(msg_str);
        return -1;
    }
    free(message_wrapper);
    free(msg_str);
    return 0;
}

int handle_send_msg(int sockfd, unsigned char *msg, unsigned char *client_id) {
    message_t *message_wrapper = create_message(MESSAGE, strlen(msg), client_id, msg);
    unsigned char *msg_str = msg_to_str(message_wrapper);
    // fprintf(stdout, "Sending message: %s\n", msg_str);
    // fprintf(stdout, "message length: %d\n", strlen(msg_str));

    int bytes = send(sockfd, msg_str, strlen(msg_str), 0);
    if (bytes == -1) {
        print_error("Failed to send message for create session.");
        free(message_wrapper);
        free(msg_str);
        return -1;
    }
    free(message_wrapper);
    free(msg_str);
    return 0;
}

