#ifndef LOGIN_H
#define LOGIN_H

/* Attempts login; on success copies username into out_user and returns 1 */
int  login(char *out_user, int out_size);

/* Change the admin password at runtime (reads/writes admin.cfg) */
void changePassword(void);

#endif
