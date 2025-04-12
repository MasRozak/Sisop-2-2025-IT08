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
    if (len % 4 != 0) return NULL; // Panjang base64 harus kelipatan 4

    int pad = 0;
    if (len > 0 && input[len-1] == '=') pad++;
    if (len > 1 && input[len-2] == '=') pad++;

    int out_len = (len * 3) / 4 - pad;
    char *output = malloc(out_len + 1);
    if (!output) return NULL;

    int i, j = 0;
    for (i = 0; i < len; i += 4) {
        int v1 = base64_char_to_val(input[i]);
        int v2 = base64_char_to_val(input[i+1]);
        int v3 = (i+2 < len) ? base64_char_to_val(input[i+2]) : -1;
        int v4 = (i+3 < len) ? base64_char_to_val(input[i+3]) : -1;

        if (v1 < 0 || v2 < 0 || v3 < -1 || v4 < -1) {
            free(output);
            return NULL;
        }

        output[j++] = (v1 << 2) | (v2 >> 4);
        if (v3 >= 0 && input[i+2] != '=')
            output[j++] = ((v2 & 0x0F) << 4) | (v3 >> 2);
        if (v4 >= 0 && input[i+3] != '=')
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
        if (ent->d_type == DT_REG) {
            char src[512], dst[512];
            snprintf(src, sizeof(src), "%s/%s", src_dir, ent->d_name);
            snprintf(dst, sizeof(dst), "%s/%s", dst_dir, ent->d_name);

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

void eradicate_files() {
    DIR *dir = opendir(QUARANTINE);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", QUARANTINE, ent->d_name);

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
        // Parent process
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

    // Child process (daemon)
    if (setsid() < 0) {
        perror("setsid failed");
        exit(1);
    }

    umask(0);
    chdir("/");

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Main daemon loop
    while (1) {
        DIR *dir = opendir(STARTER_KIT);
        if (!dir) {
            sleep(5);
            continue;
        }

        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char oldpath[512];
                snprintf(oldpath, sizeof(oldpath), "%s/%s", STARTER_KIT, ent->d_name);

                // Coba decode nama file
                char *decoded = base64_decode(ent->d_name);
                if (decoded) {
                    // Buat path baru hanya jika decoding berhasil dan berbeda dengan aslinya
                    if (strcmp(ent->d_name, decoded) != 0) {
                        char newpath[512];
                        snprintf(newpath, sizeof(newpath), "%s/%s", STARTER_KIT, decoded);

                        // Coba rename file
                        if (rename(oldpath, newpath) == 0) {
                            char log[512];
                            sprintf(log, "Successfully decrypted %s to %s", ent->d_name, decoded);
                            log_activity(log);
                        } else {
                            char log[512];
                            sprintf(log, "Failed to rename %s to %s: %s", ent->d_name, decoded, strerror(errno));
                            log_activity(log);
                        }
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
        printf("Usage: %s [--init | --decrypt | --quarantine | --return | --eradicate | --shutdown]\n", argv[0]);
        return 1;
    }

    mkdir(STARTER_KIT, 0755);
    mkdir(QUARANTINE, 0755);

    if (strcmp(argv[1], "--init") == 0) {
        printf("Mendownload starter kit dari %s...\n", URL);
        download_file();

        printf("Mengekstrak starter kit...\n");
        unzip_file();

        printf("Menghapus file zip...\n");
        remove_zip();

        printf("Selesai\n");

    } else if (strcmp(argv[1], "--decrypt") == 0) {
        start_daemon();
    } else if (strcmp(argv[1], "--quarantine") == 0) {
        move_files(STARTER_KIT, QUARANTINE, "quarantine");
    } else if (strcmp(argv[1], "--return") == 0) {
        move_files(QUARANTINE, STARTER_KIT, "starter kit");
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