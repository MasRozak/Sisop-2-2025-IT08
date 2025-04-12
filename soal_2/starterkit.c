#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define STARTER_KIT "./starter_kit"
#define QUARANTINE "./quarantine"
#define ZIP_NAME "starter.zip"
#define LOG_FILE "./activity.log"
#define PID_FILE "./decrypt.pid"

const char *URL = "https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download";

// -------------------- UTILITAS --------------------
void log_activity(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(log, "[%02d-%02d-%d][%02d:%02d:%02d] - %s\n",
                t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                t->tm_hour, t->tm_min, t->tm_sec, message);
        fclose(log);
    }
}

int base64_char_to_val(char c) {
    if ('A' <= c && c <= 'Z') return c - 'A';
    if ('a' <= c && c <= 'z') return c - 'a' + 26;
    if ('0' <= c && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

char* base64_decode(const char *input) {
    int len = strlen(input);
    if (len % 4 != 0) {
        return NULL;
    }

    int pad = 0;
    if (len >= 1 && input[len - 1] == '=') pad++;
    if (len >= 2 && input[len - 2] == '=') pad++;

    int out_len = (len * 3) / 4 - pad;
    char *output = malloc(out_len + 1);
    if (!output) return NULL;

    int i, j = 0;
    for (i = 0; i < len; i += 4) {
        int v1 = base64_char_to_val(input[i]);
        int v2 = base64_char_to_val(input[i + 1]);
        int v3 = (input[i + 2] != '=') ? base64_char_to_val(input[i + 2]) : 0;
        int v4 = (input[i + 3] != '=') ? base64_char_to_val(input[i + 3]) : 0;

        if (v1 < 0 || v2 < 0 || v3 < -1 || v4 < -1) {
            free(output);
            return NULL;
        }

        output[j++] = (v1 << 2) | (v2 >> 4);
        if (input[i + 2] != '=')
            output[j++] = ((v2 & 0x0F) << 4) | (v3 >> 2);
        if (input[i + 3] != '=')
            output[j++] = ((v3 & 0x03) << 6) | v4;
    }

    output[out_len] = '\0';
    return output;
}

// -------------------- FUNGSI OPERASI --------------------
void download_file() {
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"wget", (char *)URL, "-O", ZIP_NAME, NULL};
        execvp("wget", args);
        exit(1);
    } else {
        wait(NULL);
    }
}

void unzip_file() {
    mkdir(STARTER_KIT, 0755);
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"unzip", "-o", ZIP_NAME, "-d", STARTER_KIT, NULL};
        execvp("unzip", args);
        exit(1);
    } else {
        wait(NULL);
    }
}

void remove_zip() {
    remove(ZIP_NAME);
}

void move_files(const char *src_dir, const char *dst_dir, const char *mode) {
    DIR *dir = opendir(src_dir);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char src[512], dst[512];
        snprintf(src, sizeof(src), "%s/%s", src_dir, ent->d_name);
        snprintf(dst, sizeof(dst), "%s/%s", dst_dir, ent->d_name);

        struct stat st;
        if (stat(src, &st) == 0 && S_ISREG(st.st_mode)) {
            if (rename(src, dst) == 0) {
                char log[512];
                sprintf(log, "%s - Successfully moved to %s directory.", ent->d_name, mode);
                log_activity(log);
            } else {
                char log[512];
                sprintf(log, "Failed to move %s: %s", ent->d_name, strerror(errno));
                log_activity(log);
            }
        }
    }
    closedir(dir);
}

void copy_files(const char *src_dir, const char *dst_dir, const char *mode) {
    DIR *dir = opendir(src_dir);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char clean_name[256];
        strncpy(clean_name, ent->d_name, sizeof(clean_name) - 1);
        clean_name[sizeof(clean_name) - 1] = '\0';

        char *newline = strchr(clean_name, '\n');
        if (newline) *newline = '\0';

        char src[512], dst[512];
        snprintf(src, sizeof(src), "%s/%s", src_dir, ent->d_name);
        snprintf(dst, sizeof(dst), "%s/%s", dst_dir, clean_name);

        struct stat st;
        if (stat(src, &st) == 0 && S_ISREG(st.st_mode)) {
            FILE *in = fopen(src, "rb");
            FILE *out = fopen(dst, "wb");

            if (!in || !out) {
                char log[512];
                sprintf(log, "Failed to open files for copy %s: %s", ent->d_name, strerror(errno));
                log_activity(log);
                if (in) fclose(in);
                if (out) fclose(out);
                continue;
            }

            char buffer[4096];
            size_t bytes;
            while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
                fwrite(buffer, 1, bytes, out);
            }

            fclose(in);
            fclose(out);

            char log[512];
            sprintf(log, "%s - Successfully copied to %s directory.", clean_name, mode);
            log_activity(log);
        }
    }
    closedir(dir);
}

void eradicate_files() {
    DIR *dir = opendir(QUARANTINE);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", QUARANTINE, ent->d_name);

        struct stat st;
        if (stat(path, &st) == 0 && S_ISREG(st.st_mode)) {
            if (remove(path) == 0) {
                char log[512];
                sprintf(log, "%s - Successfully deleted.", ent->d_name);
                log_activity(log);
            } else {
                char log[512];
                sprintf(log, "Failed to delete %s: %s", ent->d_name, strerror(errno));
                log_activity(log);
            }
        }
    }
    closedir(dir);
}

void shutdown_daemon() {
    FILE *f = fopen(PID_FILE, "r");
    if (!f) {
        printf("No PID file found.\n");
        return;
    }

    int pid;
    fscanf(f, "%d", &pid);
    fclose(f);

    if (kill(pid, SIGTERM) == 0) {
        char log[256];
        sprintf(log, "Successfully shut off decryption process with PID %d.", pid);
        log_activity(log);
        remove(PID_FILE);
    } else {
        char log[256];
        sprintf(log, "Failed to kill process %d: %s", pid, strerror(errno));
        log_activity(log);
    }
}

void start_daemon() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) {
        printf("Started decryption daemon (PID: %d)\n", pid);
        FILE *f = fopen(PID_FILE, "w");
        if (f) {
            fprintf(f, "%d", pid);
            fclose(f);
        }

        char log[256];
        sprintf(log, "Successfully started decryption process with PID %d.", pid);
        log_activity(log);
        return;
    }

    if (setsid() < 0) {
        perror("setsid failed");
        exit(1);
    }

    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        DIR *dir = opendir(QUARANTINE);
        if (!dir) {
            log_activity("Failed to open STARTER_KIT directory.");
            sleep(5);
            continue;
        }

        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            char oldpath[512];
            snprintf(oldpath, sizeof(oldpath), "%s/%s", QUARANTINE, ent->d_name);

            struct stat st;
            if (stat(oldpath, &st) == 0 && S_ISREG(st.st_mode)) {
                char *decoded = base64_decode(ent->d_name);
                if (decoded && strcmp(ent->d_name, decoded) != 0) {
                    char newpath[512];
                    snprintf(newpath, sizeof(newpath), "%s/%s", QUARANTINE, decoded);

                    if (rename(oldpath, newpath) != 0) {
                        char log[512];
                        sprintf(log, "Failed to rename %s to %s: %s", ent->d_name, decoded, strerror(errno));
                        log_activity(log);
                    }
                    free(decoded);
                }
            }
        }
        closedir(dir);
        sleep(5);
    }
}

// -------------------- MAIN --------------------
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Mendownload starter kit dari %s...\n", URL);
        download_file();

        printf("Mengekstrak starter kit...\n");
        unzip_file();

        printf("Menghapus file zip...\n");
        remove_zip();

        printf("Selesai\n");

        printf("Usage: %s [--decrypt | --quarantine | --return | --eradicate | --shutdown]\n", argv[0]);
        return 1;
    }

    mkdir(STARTER_KIT, 0755);
    mkdir(QUARANTINE, 0755);

    if (strcmp(argv[1], "--decrypt") == 0) {
        start_daemon();
    } else if (strcmp(argv[1], "--quarantine") == 0) {
        move_files(STARTER_KIT, QUARANTINE, "quarantine");
    } else if (strcmp(argv[1], "--return") == 0) {
        copy_files(QUARANTINE, STARTER_KIT, "starter kit");
    } else if (strcmp(argv[1], "--eradicate") == 0) {
        eradicate_files();
    } else if (strcmp(argv[1], "--shutdown") == 0) {
        shutdown_daemon();
    } else {
        printf("Invalid option.\n");
        return 1;
    }

    return 0;
}
