#ifndef LOGIN_H
#define LOGIN_H

/* Attempts login; on success copies username into out_user and returns 1 */
int login(char *out_user, int out_size);

#endif
