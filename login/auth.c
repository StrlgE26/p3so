#define _POSIX_C_SOURCE 200809L
#include "auth.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define USER_DB_PATH "usuarios.txt"

int auth_valid_username(const char *username)
{
    size_t n;
    if (!username) return 0;
    n = strlen(username);
    if (n < USERNAME_MIN || n > USERNAME_MAX) return 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)username[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-')) return 0;
    }
    return 1;
}

int auth_valid_email(const char *email)
{
    const char *at, *dot;
    size_t n;
    if (!email) return 0;
    n = strlen(email);
    if (n < 6 || n > EMAIL_MAX || strchr(email, ' ') || strchr(email, '\t')) return 0;
    at = strchr(email, '@');
    if (!at || at == email || strchr(at + 1, '@')) return 0;
    if (at[1] == '\0' || at[1] == '.') return 0;
    dot = strrchr(at + 1, '.');
    if (!dot || dot == at + 1 || dot[1] == '\0' || strlen(dot + 1) < 2) return 0;
    if (strchr(email, ':') || strchr(email, '\n') || strchr(email, '\r')) return 0;
    for (const unsigned char *p = (const unsigned char *)email; *p; ++p) {
        if (!(isalnum(*p) || strchr(".!#$%&'*+/=?^_`{|}~-@", *p))) return 0;
    }
    return 1;
}

int auth_valid_password(const char *password)
{
    int upper = 0, lower = 0, digit = 0, symbol = 0;
    size_t n;
    if (!password) return 0;
    n = strlen(password);
    if (n < PASSWORD_MIN || n > PASSWORD_MAX) return 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)password[i];
        if (c < 33 || c > 126) return 0;
        if (c >= 'A' && c <= 'Z') upper = 1;
        else if (c >= 'a' && c <= 'z') lower = 1;
        else if (c >= '0' && c <= '9') digit = 1;
        else symbol = 1;
    }
    return upper && lower && digit && symbol;
}

/* File records: username:email:Argon2id-hash. Legacy username:hash records
 * remain readable for login, but new registrations use the new format. */
static int find_existing_locked(int fd, const char *username, const char *email,
                                int *user_exists, int *email_exists)
{
    FILE *fp;
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int dupfd, error = 0;
    *user_exists = *email_exists = 0;
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) return -1;
    dupfd = dup(fd);
    if (dupfd < 0) return -1;
    fp = fdopen(dupfd, "r");
    if (!fp) { close(dupfd); return -1; }
    while ((len = getline(&line, &cap, fp)) != -1) {
        char *first, *second;
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
        first = strchr(line, ':');
        if (!first) continue;
        *first++ = '\0';
        if (strcmp(line, username) == 0) *user_exists = 1;
        second = strchr(first, ':');
        if (second) {
            *second = '\0';
            if (*first && strcmp(first, email) == 0) *email_exists = 1;
        }
    }
    if (ferror(fp)) error = 1;
    free(line);
    if (fclose(fp) != 0) error = 1;
    return error ? -1 : 0;
}

static int write_all(int fd, const char *s, size_t n)
{
    size_t done = 0;
    while (done < n) {
        ssize_t r = write(fd, s + done, n - done);
        if (r < 0 && errno == EINTR) continue;
        if (r <= 0) return -1;
        done += (size_t)r;
    }
    return 0;
}

AuthResult auth_register_user(const char *username, const char *email, const char *password)
{
    char hash[crypto_pwhash_STRBYTES];
    char record[USERNAME_MAX + EMAIL_MAX + crypto_pwhash_STRBYTES + 4];
    int fd, ue, ee, n;
    AuthResult result = AUTH_IO_ERROR;
    if (!auth_valid_username(username) || !auth_valid_email(email) || !auth_valid_password(password))
        return AUTH_INVALID_INPUT;
    if (crypto_pwhash_str(hash, password, (unsigned long long)strlen(password),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        sodium_memzero(hash, sizeof hash); return AUTH_CRYPTO_ERROR;
    }
    fd = open(USER_DB_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) { sodium_memzero(hash, sizeof hash); return AUTH_IO_ERROR; }
    if (fchmod(fd, S_IRUSR | S_IWUSR) < 0 || flock(fd, LOCK_EX) < 0) {
        close(fd); sodium_memzero(hash, sizeof hash); return AUTH_IO_ERROR;
    }
    if (find_existing_locked(fd, username, email, &ue, &ee) < 0) goto done;
    if (ue) { result = AUTH_USER_EXISTS; goto done; }
    if (ee) { result = AUTH_EMAIL_EXISTS; goto done; }
    n = snprintf(record, sizeof record, "%s:%s:%s\n", username, email, hash);
    if (n < 0 || (size_t)n >= sizeof record || lseek(fd, 0, SEEK_END) < 0 ||
        write_all(fd, record, (size_t)n) < 0 || fsync(fd) < 0) goto done;
    result = AUTH_OK;
done:
    flock(fd, LOCK_UN); close(fd);
    sodium_memzero(hash, sizeof hash); sodium_memzero(record, sizeof record);
    return result;
}

AuthResult auth_login_user(const char *username, const char *password)
{
    FILE *fp;
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    AuthResult result = AUTH_USER_NOT_FOUND;
    /* Login does not enforce registration policy; it only requires usable input. */
    if (!username || !*username || strlen(username) > USERNAME_MAX ||
        !password || !*password || strlen(password) > PASSWORD_MAX)
        return AUTH_INVALID_INPUT;
    fp = fopen(USER_DB_PATH, "r");
    if (!fp) return errno == ENOENT ? AUTH_USER_NOT_FOUND : AUTH_IO_ERROR;
    while ((len = getline(&line, &cap, fp)) != -1) {
        char *first, *hash;
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
        first = strchr(line, ':'); if (!first) continue;
        *first++ = '\0';
        if (strcmp(line, username) != 0) continue;
        hash = strchr(first, ':');
        if (hash) hash++; else hash = first; /* legacy username:hash */
        result = crypto_pwhash_str_verify(hash, password, (unsigned long long)strlen(password)) == 0
                 ? AUTH_OK : AUTH_WRONG_PASSWORD;
        break;
    }
    if (ferror(fp)) result = AUTH_IO_ERROR;
    free(line);
    if (fclose(fp) != 0 && result == AUTH_OK) result = AUTH_IO_ERROR;
    return result;
}
