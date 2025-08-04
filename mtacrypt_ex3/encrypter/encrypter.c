#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../common/mtacrypt.h"

#define MAX_DECRYPTERS 10

typedef struct {
    char pipe_name[128];
} DecrypterInfo;

DecrypterInfo decrypters[MAX_DECRYPTERS];
int decrypter_count = 0;
char current_password[MAX_PASSWORD_LENGTH + 1];
int password_length = 6;

void load_config() {
    FILE *f = fopen(CONFIG_FILE, "r");
    if (!f) {
        log_message("Cannot open config file");
        exit(1);
    }
    char line[100];
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "PASSWORD_LENGTH=%d", &password_length) == 1) {
            if (password_length > MAX_PASSWORD_LENGTH) password_length = MAX_PASSWORD_LENGTH;
            break;
        }
    }
    fclose(f);
}

void broadcast_password() {
    for (int i = 0; i < decrypter_count; i++) {
        int fd = open(decrypters[i].pipe_name, O_WRONLY | O_NONBLOCK);
        if (fd >= 0) {
            write(fd, current_password, password_length);
            close(fd);
            log_message("Sent password '%s' to %s", current_password, decrypters[i].pipe_name);
        } else {
            log_message("Failed to write password to %s", decrypters[i].pipe_name);
        }
    }
}

void handle_subscription(const char *pipe_name) {
    if (decrypter_count < MAX_DECRYPTERS) {
        strcpy(decrypters[decrypter_count].pipe_name, pipe_name);
        decrypter_count++;
        log_message("Decrypter subscribed: %s", pipe_name);
    } else {
        log_message("Max decrypters reached. Cannot subscribe: %s", pipe_name);
    }
}

int main() {
    mkfifo(ENCRYPTER_PIPE, 0666);
    load_config();
    generate_password(current_password, password_length);
    log_message("Encrypter started, password: %s", current_password);

    int fd = open(ENCRYPTER_PIPE, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        log_message("Failed to open ENCRYPTER_PIPE for reading");
        exit(1);
    }

    log_message("Waiting for decrypters to subscribe...");
    while (decrypter_count == 0) {
        char buf[256];
        int n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            if (strncmp(buf, "SUBSCRIBE:", 10) == 0) {
                handle_subscription(buf + 10);
            }
        }
        sleep(1);
    }

    broadcast_password();

    char buf[256];
    while (1) {
        int n = read(fd, buf, sizeof(buf) - 1);
        if (n <= 0) {
            sleep(1);
            continue;
        }
        buf[n] = '\0';

        if (strncmp(buf, "SUBSCRIBE:", 10) == 0) {
            handle_subscription(buf + 10);
            int pipefd = open(buf + 10, O_WRONLY);
            if (pipefd >= 0) {
                write(pipefd, current_password, password_length);
                close(pipefd);
                log_message("Sent password '%s' to new subscriber %s", current_password, buf + 10);
            } else {
                log_message("Failed to open pipe %s for writing", buf + 10);
            }

        } else if (strncmp(buf, "DECRYPTED:", 10) == 0) {
            if (strncmp(buf + 10, current_password, password_length) == 0) {
                log_message("Correct password received: %s", buf + 10);
                generate_password(current_password, password_length);
                log_message("New password: %s", current_password);
                broadcast_password();
            } else {
                log_message("Wrong password tried: %s", buf + 10);
            }
        }
    }

    close(fd);
    return 0;
}

