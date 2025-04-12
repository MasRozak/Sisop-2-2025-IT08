#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

void write_log(const char *username, const char *proc_name, const char *status) {
    char path[512];
    snprintf(path, sizeof(path), "/home/%s/debugmon.log", username);
    FILE *log = fopen(path, "a");
    if (!log) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d:%02d:%04d]-%02d:%02d:%02d_%s_%s\n",
        t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        proc_name, status);
    fclose(log);
}

void list_by_uid(uid_t uid) {
    DIR *proc = opendir("/proc");
    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;
        char status_path[256];
        snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);

        FILE *status = fopen(status_path, "r");
        if (!status) continue;

        uid_t file_uid = -1;
        char name[256] = "";
        char line[256];
        while (fgets(line, sizeof(line), status)) {
            if (strncmp(line, "Uid:", 4) == 0) {
                sscanf(line, "Uid:\t%d", &file_uid);
            } else if (strncmp(line, "Name:", 5) == 0) {
                sscanf(line, "Name:\t%255s", name);
            }
        }
        fclose(status);

        if (file_uid == uid) {
            printf("PID: %s, Command: %s\n", entry->d_name, name);
        }
    }
    closedir(proc);
}

void fail_user(uid_t uid, const char *username) {
    DIR *proc = opendir("/proc");
    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;
        char status_path[256];
        snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);

        FILE *status = fopen(status_path, "r");
        if (!status) continue;

        uid_t file_uid = -1;
        char name[256] = "";
        char line[256];
        while (fgets(line, sizeof(line), status)) {
            if (strncmp(line, "Uid:", 4) == 0) {
                sscanf(line, "Uid:\t%d", &file_uid);
            } else if (strncmp(line, "Name:", 5) == 0) {
                sscanf(line, "Name:\t%255s", name);
            }
        }
        fclose(status);

        if (file_uid == uid) {
            kill(atoi(entry->d_name), SIGKILL);
            write_log(username, name, "FAILED");
        }
    }
    closedir(proc);
}

void revert_user(const char *username) {
    write_log(username, "revert", "RUNNING");
}

void daemonize(const char *username, uid_t uid) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) {
        // save child pid to file
        char path[256];
        snprintf(path, sizeof(path), "/tmp/debugmon_%s.pid", username);
        FILE *f = fopen(path, "w");
        if (f) {
            fprintf(f, "%d", pid);
            fclose(f);
        }
        return;
    }

    setsid();
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    chdir("/");
    umask(0);
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    while (1) {
        DIR *proc = opendir("/proc");
        struct dirent *entry;
        while ((entry = readdir(proc)) != NULL) {
            if (!isdigit(entry->d_name[0])) continue;
            char status_path[256];
            snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);

            FILE *status = fopen(status_path, "r");
            if (!status) continue;

            uid_t file_uid = -1;
            char name[256] = "";
            char line[256];
            while (fgets(line, sizeof(line), status)) {
                if (strncmp(line, "Uid:", 4) == 0) {
                    sscanf(line, "Uid:\t%d", &file_uid);
                } else if (strncmp(line, "Name:", 5) == 0) {
                    sscanf(line, "Name:\t%255s", name);
                }
            }
            fclose(status);

            if (file_uid == uid) {
                write_log(username, name, "RUNNING");
            }
        }
        closedir(proc);
        sleep(5);
    }
}

void stop_daemon(const char *username) {
    char path[256];
    snprintf(path, sizeof(path), "/tmp/debugmon_%s.pid", username);
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Daemon untuk user %s tidak ditemukan.\n", username);
        return;
    }
    int pid;
    fscanf(f, "%d", &pid);
    fclose(f);
    if (kill(pid, SIGTERM) == 0) {
        printf("Daemon untuk user %s berhasil dihentikan.\n", username);
        remove(path);
    } else {
        perror("Gagal menghentikan daemon");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <command> <username>\n", argv[0]);
        return 1;
    }

    struct passwd *pw = getpwnam(argv[2]);
    if (!pw) {
        fprintf(stderr, "User '%s' tidak ditemukan.\n", argv[2]);
        return 1;
    }

    uid_t uid = pw->pw_uid;

    if (strcmp(argv[1], "list") == 0) {
        list_by_uid(uid);
    } else if (strcmp(argv[1], "fail") == 0) {
        fail_user(uid, argv[2]);
    } else if (strcmp(argv[1], "revert") == 0) {
        revert_user(argv[2]);
    } else if (strcmp(argv[1], "daemon") == 0) {
        daemonize(argv[2], uid);
    } else if (strcmp(argv[1], "stop") == 0) {
        stop_daemon(argv[2]);
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}