#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_PID 65536
time_t last_logged[MAX_PID] = { 0 };
bool seen_pids[MAX_PID] = { false };

struct passwd *get_user_info(const char *username) {
    struct passwd *pw = getpwnam(username);
    if (!pw) fprintf(stderr, "User '%s' not found\n", username);
    return pw;
}

void get_paths(const char *home, const char *username, char *log_path, char *pid_path, char *flag_path) {
    if (log_path) snprintf(log_path, 512, "%s/debugmon.log", home);
    if (pid_path) snprintf(pid_path, 512, "%s/.debugmon_%s.pid", home, username);
    if (flag_path) snprintf(flag_path, 512, "%s/.debugmon_fail_flag", home);
}

// Tulis log ke file
void write_log(const char *log_path, const char *process_name, const char *status) {
    FILE *log = fopen(log_path, "a");
    if (!log) return;

    char timestamp[64];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%d:%m:%Y-%H:%M:%S", localtime(&now));
    fprintf(log, "[%s]_%s_%s\n", timestamp, process_name, status);
    fclose(log);
}

// Cek apakah string adalah angka
int is_number(const char *s) {
    for (int i = 0; s[i]; i++)
        if (!isdigit(s[i])) return 0;
    return 1;
}

// Ambil info UID dan nama proses dari /proc/[pid]/status
bool get_process_info(const char *pid, uid_t *uid, char *name, size_t name_size) {
    char status_path[256];
    snprintf(status_path, sizeof(status_path), "/proc/%s/status", pid);

    FILE *f = fopen(status_path, "r");
    if (!f) return false;

    char line[256];
    *uid = -1;
    strcpy(name, "Unknown");

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "Name:", 5) == 0)
            sscanf(line, "Name:\t%255s", name);
        else if (strncmp(line, "Uid:", 4) == 0) {
            sscanf(line, "Uid:\t%u", uid);
            break;
        }
    }

    fclose(f);
    return (*uid != (uid_t)-1);
}

// Hitung CPU usage (%) dan memory (KB)
bool get_cpu_mem_usage(const char *pid, float *cpu_usage, long *mem_kb) {
    char path[256];
    FILE *f;

    // Ambil uptime sistem
    float uptime = 0;
    f = fopen("/proc/uptime", "r");
    if (!f) return false;
    fscanf(f, "%f", &uptime);
    fclose(f);

    // Ambil /proc/[pid]/stat
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);
    f = fopen(path, "r");
    if (!f) return false;

    long utime, stime, starttime;
    int i = 0;
    char comm[256], state;
    unsigned long dummy;
    int pid_i;
    fscanf(f, "%d %s %c", &pid_i, comm, &state);
    for (i = 3; i <= 13; i++) fscanf(f, "%lu", &dummy); // skip
    fscanf(f, "%lu %lu", &utime, &stime);
    for (i = 16; i <= 21; i++) fscanf(f, "%lu", &dummy); // skip
    fscanf(f, "%lu", &starttime);
    fclose(f);

    long clk_tck = sysconf(_SC_CLK_TCK);
    float total_time = (float)(utime + stime) / clk_tck;
    float seconds = uptime - ((float)starttime / clk_tck);
    *cpu_usage = seconds > 0 ? 100 * (total_time / seconds) : 0;

    // Ambil RSS dari /proc/[pid]/statm
    snprintf(path, sizeof(path), "/proc/%s/statm", pid);
    f = fopen(path, "r");
    if (!f) return false;
    long rss;
    fscanf(f, "%*ld %ld", &rss); // skip size
    fclose(f);

    long page_size = sysconf(_SC_PAGESIZE);
    *mem_kb = rss * page_size / 1024;
    return true;
}

void handle_list(const char *username) {
    struct passwd *pw = get_user_info(username);
    if (!pw) return;

    uid_t uid_target = pw->pw_uid;
    DIR *proc = opendir("/proc");
    if (!proc) return;

    struct dirent *entry;
    printf("%-8s %-20s %-10s %-10s\n", "PID", "COMMAND", "CPU(%)", "MEM(KB)");
    while ((entry = readdir(proc))) {
        if (is_number(entry->d_name)) {
            uid_t uid;
            char name[256];
            if (get_process_info(entry->d_name, &uid, name, sizeof(name)) && uid == uid_target) {
                float cpu;
                long mem;
                if (get_cpu_mem_usage(entry->d_name, &cpu, &mem))
                    printf("%-8s %-20s %-10.2f %-10ld\n", entry->d_name, name, cpu, mem);
            }
        }
    }

    closedir(proc);
}

bool is_failing_mode(const char *home) {
    char flag_path[512];
    get_paths(home, NULL, NULL, NULL, flag_path);

    FILE *flag = fopen(flag_path, "r");
    if (!flag) return false;

    char mode[16];
    fscanf(flag, "%15s", mode);
    fclose(flag);
    return strcmp(mode, "FAILING") == 0;
}

void monitor_user(uid_t uid_target, const char *log_path, bool fail_mode) {
    DIR *proc = opendir("/proc");
    if (!proc) return;

    struct dirent *entry;
    while ((entry = readdir(proc))) {
        if (is_number(entry->d_name)) {
            uid_t uid;
            char name[256];
            if (get_process_info(entry->d_name, &uid, name, sizeof(name)) && uid == uid_target) {
                int pid = atoi(entry->d_name);
                if (pid < MAX_PID && !seen_pids[pid]) {
                    seen_pids[pid] = true;
                    if (fail_mode) {
                        write_log(log_path, name, "FAILED");
                        kill(pid, SIGKILL);
                    }
                }
            }
        }
    }

    closedir(proc);
}

void handle_daemon(const char *username) {
    struct passwd *pw = get_user_info(username);
    if (!pw) return;

    uid_t uid_target = pw->pw_uid;
    char log_path[512], pid_path[512];
    get_paths(pw->pw_dir, username, log_path, pid_path, NULL);

    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Parent

    setsid();
    chdir("/");
    close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);

    FILE *pid_file = fopen(pid_path, "w");
    if (pid_file) {
        fprintf(pid_file, "%d\n", getpid());
        fclose(pid_file);
    }

    while (1) {
        monitor_user(uid_target, log_path, is_failing_mode(pw->pw_dir));
        sleep(1);
    }
}

void handle_stop(const char *username) {
    struct passwd *pw = get_user_info(username);
    if (!pw) return;

    char pid_path[512], log_path[512];
    get_paths(pw->pw_dir, username, log_path, pid_path, NULL);

    FILE *pid_file = fopen(pid_path, "r");
    if (!pid_file) {
        fprintf(stderr, "PID file not found. Is daemon running?\n");
        return;
    }

    pid_t pid;
    fscanf(pid_file, "%d", &pid);
    fclose(pid_file);

    if (kill(pid, SIGTERM) == 0) {
        write_log(log_path, "debugmon-stop", "RUNNING");
        remove(pid_path);
        printf("Daemon for %s (PID %d) terminated.\n", username, pid);
    } else {
        perror("Failed to kill daemon");
    }
}

void handle_fail(const char *username) {
    struct passwd *pw = get_user_info(username);
    if (!pw) return;

    uid_t uid_target = pw->pw_uid;
    char log_path[512], flag_path[512];
    get_paths(pw->pw_dir, username, log_path, NULL, flag_path);

    DIR *proc = opendir("/proc");
    if (!proc) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc))) {
        if (is_number(entry->d_name)) {
            uid_t uid;
            char name[256];
            if (get_process_info(entry->d_name, &uid, name, sizeof(name)) && uid == uid_target) {
                pid_t pid = atoi(entry->d_name);
                if (kill(pid, SIGKILL) == 0)
                    write_log(log_path, name, "FAILED");
            }
        }
    }

    closedir(proc);

    FILE *flag = fopen(flag_path, "w");
    if (flag) {
        fprintf(flag, "FAILING\n");
        fclose(flag);
    }

    printf("Semua proses user '%s' digagalkan. Mode FAILING aktif.\n", username);
    handle_daemon(username);
}

void handle_revert(const char *username) {
    struct passwd *pw = get_user_info(username);
    if (!pw) return;

    char log_path[512], flag_path[512];
    get_paths(pw->pw_dir, username, log_path, NULL, flag_path);
    remove(flag_path);
    write_log(log_path, "debugmon-revert", "RUNNING");
    printf("Mode FAILING dimatikan untuk user '%s'.\n", username);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <command> <username>\n", argv[0]);
        return 1;
    }

    const char *cmd = argv[1], *user = argv[2];

    if (strcmp(cmd, "list") == 0)
        handle_list(user);
    else if (strcmp(cmd, "daemon") == 0) {
        struct passwd *pw = get_user_info(user);
        if (!pw) exit(EXIT_FAILURE);
        char log_path[512];
        get_paths(pw->pw_dir, user, log_path, NULL, NULL);
        write_log(log_path, "debugmon-daemon", "RUNNING");
        handle_daemon(user);
    } else if (strcmp(cmd, "stop") == 0)
        handle_stop(user);
    else if (strcmp(cmd, "fail") == 0)
        handle_fail(user);
    else if (strcmp(cmd, "revert") == 0)
        handle_revert(user);
    else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        return 1;
    }

    return 0;
}
