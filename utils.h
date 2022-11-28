#ifndef UTILS_H
#define UTILS_H

// LOGIN
// message data: <password>
// Login with the server
#define LOGIN 0

// LO_ACK
// message data: can be none
// Acknowledge successful login
#define LO_ACK 1

// LO_NAK
// message data: <reason for failure>
// Negative acknowledgement of login
#define LO_NAK 2
#define USER_DNE 100
#define USER_ALREADY_LOGGED_IN 101
#define INCORRECT_PASSWORD 102
#define LOGIN_RESPONSE_LEN 45

// EXIT
// message data: can be none
// Exit from the server
#define EXIT 3
#define LOGOUT_ACK 13
// JOIN_SESSION
// message data: <session ID>
// Join a conference session
#define JOIN_SESSION 4

// JN_ACK
// message data: <session ID>
// Acknowledge successful conference session join
#define JOIN_ACK 5

// JN_NAK
// message data: <session ID, reason for failure>
// Negative acknowledgement of joining the session
#define JOIN_NACK 6
#define USER_NOT_FOUND 103
#define SESSION_NOT_FOUND 104
// for lab 4, each user can only be in 1 session
#define USER_ALREADY_IN_SESSION 105

// LEAVE_SES S
// message data: can be none
// Leave a conference session2
#define LEAVE_SESSION 7
#define LEAVE_ACK 14
#define LEAVE_NACK 15

// NEW_SESS
// message data: <session ID>
// Create new conference session
#define NEW_SESSION 8
#define SESSION_ID_EXISTS 106

// register
#define REGISTER 17
#define REGISTER_RESPONSE_LEN 45
#define USERNAME_TAKEN 107
#define MAX_USER_EXCEEDED 108
#define REGISTER_SUCCESS 109

// NS_ACK
// message data: can be none
// Acknowledge new conference session
#define NEW_SESSION_ACK 9
#define NEW_SESSION_NACK 16

// MESSAGE
// message data: <message data>
// Send a message to the session or display the message if it is received
#define MESSAGE 10

// QUERY
// Get a list of online users and available sessions
#define QUERY 11

// QU_ACK
// <users and sessions>
// Reply followed by a list of users online
#define QUERY_ACK 12

#define MAX_NAME 128
// data can be super long due to /list
#define MAX_DATA 160000
#define MAX_MSG_STR 161024

#define bold "\e[1m"
#define bold_off "\e[m"
#define red "\033[0;31m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define bold_blue "\e[1;34m"
#define bold_green "\e[1;32m"
#define black "\033[0m"
/************************************
 * format   ->  type:size:source:data
 * type     ->  one of the 13 types.
 * size     ->  size of data.
 * source   ->  contains ID of the client sending the message.
 * data     ->  the data being sent.
 *************************************/ 

typedef struct message {
    unsigned int type;
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
} message_t;

#endif
