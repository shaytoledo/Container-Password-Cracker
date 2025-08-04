#ifndef MTACRYPT_H
#define MTACRYPT_H
#define MAX_PASSWORD_LENGTH 64
#define PIPE_DIR "/mnt/mta"
#define ENCRYPTER_PIPE "/mnt/mta/encrypter_pipe"
#define CONFIG_FILE "/mnt/mta/config.txt"
#define LOG_FILE "/var/log/mtacrypt.log"

void generate_password(char *buf, int len); 
void log_message(const char *fmt, ...);

#endif

