#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "../common/mtacrypt.h"


char my_pipe[128];
char password[MAX_PASSWORD_LENGTH + 1];
int password_length = 6;

const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
int charset_size = sizeof(charset) - 1;

char try[MAX_PASSWORD_LENGTH + 1];
int found = 0;

void log_message(const char *format, ...) {
    char filename[256];
    snprintf(filename, sizeof(filename), "/var/log/decrypter_%d.log", getpid());

    FILE *f = fopen(filename, "a");
    if (!f) return;

    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    fprintf(f, "\n");
    va_end(args);

    fclose(f);
}


void find_next_pipe() {
    int idx = 1;
    while (1) {
        snprintf(my_pipe, sizeof(my_pipe), "%s/decrypter_pipe_%d", PIPE_DIR, idx);
        if (access(my_pipe, F_OK) == -1) break;
        idx++;
    }
    mkfifo(my_pipe, 0666);
}

void subscribe() {
    int fd = open(ENCRYPTER_PIPE, O_WRONLY);
    char msg[256];
    sprintf(msg, "SUBSCRIBE:%s", my_pipe);
    write(fd, msg, strlen(msg));
    close(fd);
    log_message("Subscribed as %s", my_pipe);
}

void send_decrypted(const char *try_pwd) {
    int fd = open(ENCRYPTER_PIPE, O_WRONLY);
    if (fd >= 0) {
        char msg[256];
        sprintf(msg, "DECRYPTED:%s", try_pwd);
        write(fd, msg, strlen(msg));
        close(fd);
    }
}

void brute_force(int pos) {
    if (found) return;
    if (pos == password_length) {
        try[pos] = '\0';
        send_decrypted(try);
        if (strcmp(try, password) == 0) {
            found = 1;
        }
        return;
    }
    for (int i = 0; i < charset_size; i++) {
        try[pos] = charset[i];
        brute_force(pos + 1);
        if (found) return;
    }
}

int main() {
    find_next_pipe();
    subscribe();
    int fd = open(my_pipe, O_RDONLY);
    while (1) {
        int n = read(fd, password, sizeof(password) - 1);
        if (n > 0) {
            password[n] = 0;
            password_length = strlen(password);
            log_message("Received password: %s", password);
            break;
        }
        sleep(1);
    }
    brute_force(0);
    close(fd);
    unlink(my_pipe);
    return 0;
}

