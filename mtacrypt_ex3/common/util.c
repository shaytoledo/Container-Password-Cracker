#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "mtacrypt.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>



void log_message(const char *fmt, ...) {
    FILE *f = fopen(LOG_FILE, "a");
    if (!f) return;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    fprintf(f, "[%02d:%02d:%02d] ", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fflush(f); 
    fclose(f);
}


void generate_password(char *buf, int len) {
    static const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int charset_size = sizeof(charset) - 1;

    srand(time(NULL) ^ getpid());

    for (int i = 0; i < len; ++i) {
        buf[i] = charset[rand() % charset_size];
    }
    buf[len] = '\0';

}
