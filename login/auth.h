#ifndef AUTH_H
#define AUTH_H

#include <stddef.h>

#define USERNAME_MIN 5
#define USERNAME_MAX 24
#define EMAIL_MAX 254
#define PASSWORD_MIN 8
#define PASSWORD_MAX 128

typedef enum {
    AUTH_OK = 0,
    AUTH_INVALID_INPUT,
    AUTH_USER_EXISTS,
    AUTH_EMAIL_EXISTS,
    AUTH_USER_NOT_FOUND,
    AUTH_WRONG_PASSWORD,
    AUTH_IO_ERROR,
    AUTH_CRYPTO_ERROR
} AuthResult;

AuthResult auth_register_user(const char *username, const char *email,
                              const char *password);
AuthResult auth_login_user(const char *username, const char *password);
int auth_valid_username(const char *username);
int auth_valid_email(const char *email);
int auth_valid_password(const char *password);

#endif
