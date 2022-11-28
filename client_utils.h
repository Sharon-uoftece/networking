#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

// client utils
void print_blue_arrow();
void print_bold_arrow();
int is_str_equal(unsigned char *s1, unsigned char *s2, int length);
int is_number_str(unsigned char *str);
void print_error(unsigned char *str);
message_t *create_message(unsigned type, unsigned size, unsigned char *source, unsigned char *data);
unsigned char *msg_to_str(message_t* msg);
void free_message(message_t *msg);
int read_from_stream(int sockfd, unsigned char *buf);
int handle_login(unsigned char *msg, unsigned char *client_id);
int handle_register(unsigned char *msg, unsigned char *client_id);
int handle_logout(int sockfd, unsigned char *client_id);
int handle_join_session(int sockfd, unsigned char *msg, unsigned char *client_id);
int handle_leave_session(int sockfd, unsigned char *client_id);
int handle_create_session(int sockfd, unsigned char *msg, unsigned char *client_id);
int handle_list(int sockfd, unsigned char *client_id);
int handle_send_msg(int sockfd, unsigned char *msg, unsigned char *client_id);

#endif
