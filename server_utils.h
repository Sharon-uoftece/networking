#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#define MAX_USERS 16
#define MAX_SESSIONS 16
#define MAX_SESSION_USERS 16

extern int session_id;

typedef struct user {
    int id;
    int session_id;
    int sockfd;
    pthread_mutex_t mutex_sockfd;
    // for user to log in
    unsigned char client_name[MAX_NAME];
    unsigned char password[MAX_NAME];
} user_t;

typedef struct session {
    int id;
    int users[MAX_SESSION_USERS];
    unsigned int num_users;
    // for clients to join session
    unsigned char name[MAX_NAME];
} session_t;

message_t *create_message(unsigned type, unsigned size, unsigned char *source, unsigned char *data);
unsigned char *msg_to_str(message_t* msg);
int is_str_equal(unsigned char *s1, unsigned char *s2, int length);
int is_number_str(unsigned char *str);
void print_error(unsigned char *str);
void print_usage();
int read_from_stream(int sockfd, unsigned char *buf);
int handle_login(int sockfd, unsigned char *msg, user_t *known_users[], user_t *active_users[]);
int handle_register(int sockfd, unsigned char *msg, user_t *known_users[], user_t *active_users[]);
int handle_logout(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]);
int handle_create_sesssion(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]);
int handle_join_session(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]);
int handle_leave_session(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]);
int handle_message(unsigned char *msg, user_t *active_users[], session_t *active_sessions[]);
unsigned char *handle_query(user_t *known_users[], user_t *active_users[], session_t *active_sessions[]);
#endif
