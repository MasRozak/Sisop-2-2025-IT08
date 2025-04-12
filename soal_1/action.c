#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define CLUE_DIR "Clues"
#define FILTERED_DIR "Filtered"

int is_digit_filename(const char *filename) {
    return strlen(filename) == 5 && isdigit(filename[0]) && strcmp(filename + 1, ".txt") == 0;
}

int is_alpha_filename(const char *filename) {
    return strlen(filename) == 5 && isalpha(filename[0]) && strcmp(filename + 1, ".txt") == 0;
}

int cmp_filename(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}


int is_valid_file(const char *name) {
    int len = strlen(name);
    return len == 5 &&
           name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't' &&
           isalnum(name[0]);
}

void download_and_extract() {
    struct stat st;
    if (stat(CLUE_DIR, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Folder %s already exists. Skipping download.\n", CLUE_DIR);
        return;
    }
    printf("Downloading Clues.zip...\n");
    system("wget -q --show-progress \"https://drive.usercontent.google.com/u/0/uc?id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK&export=download\" -O Clues.zip");
    system("unzip -q Clues.zip");
    system("rm Clues.zip");
    printf("Downloaded and extracted Clues.zip\n");
}

void filter_files() {
    DIR *d;
    struct dirent *dir;
    mkdir(FILTERED_DIR, 0755);

    char *folders[] = {"Clues/ClueA", "Clues/ClueB", "Clues/ClueC", "Clues/ClueD"};
    for (int i = 0; i < 4; i++) {
        d = opendir(folders[i]);
        if (!d) continue;

        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            char src_path[256];
            snprintf(src_path, sizeof(src_path), "%s/%s", folders[i], dir->d_name);

            if (is_valid_file(dir->d_name)) {
                char dest_path[256];
                snprintf(dest_path, sizeof(dest_path), "%s/%s", FILTERED_DIR, dir->d_name);

                FILE *src = fopen(src_path, "r");
                FILE *dest = fopen(dest_path, "w");
                if (src && dest) {
                    char ch;
                    while ((ch = fgetc(src)) != EOF)
                        fputc(ch, dest);
                }
                if (src) fclose(src);
                if (dest) fclose(dest);
            }
        }
        closedir(d);
    }

    for (int i = 0; i < 4; i++) {
        d = opendir(folders[i]);
        if (!d) continue;
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            char del_path[256];
            snprintf(del_path, sizeof(del_path), "%s/%s", folders[i], dir->d_name);
            remove(del_path);
        }
        closedir(d);
    }

    printf("Filtering completed. Original clue files removed.\n");
}


int cmp(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void combine_files() {
    DIR *d = opendir(FILTERED_DIR);
    struct dirent *dir;
    char *digits[100], *alphas[100];
    int d_count = 0, a_count = 0;

    if (!d) {
        perror("Could not open Filtered directory");
        return;
    }

    while ((dir = readdir(d)) != NULL) {
        if (is_digit_filename(dir->d_name)) {
            digits[d_count++] = strdup(dir->d_name);
        } else if (is_alpha_filename(dir->d_name)) {
            alphas[a_count++] = strdup(dir->d_name);
        }
    }
    closedir(d);

    qsort(digits, d_count, sizeof(char *), cmp_filename);
    qsort(alphas, a_count, sizeof(char *), cmp_filename);

    FILE *combined = fopen("Combined.txt", "w");
    if (!combined) {
        perror("Failed to create Combined.txt");
        return;
    }

    int i = 0, j = 0;
    while (i < d_count || j < a_count) {
        if (i < d_count) {
            char path[256];
            snprintf(path, sizeof(path), "%s/%s", FILTERED_DIR, digits[i]);
            FILE *f = fopen(path, "r");
            if (f) {
                char ch;
                while ((ch = fgetc(f)) != EOF)
                    fputc(ch, combined);
                fclose(f);
                remove(path);
            }
            i++;
        }

        if (j < a_count) {
            char path[256];
            snprintf(path, sizeof(path), "%s/%s", FILTERED_DIR, alphas[j]);
            FILE *f = fopen(path, "r");
            if (f) {
                char ch;
                while ((ch = fgetc(f)) != EOF)
                    fputc(ch, combined);
                fclose(f);
                remove(path);
            }
            j++;
        }
    }

    fclose(combined);
    printf("Combined.txt created.\n");
}

char rot13(char c) {
    if ('a' <= c && c <= 'z') return 'a' + (c - 'a' + 13) % 26;
    if ('A' <= c && c <= 'Z') return 'A' + (c - 'A' + 13) % 26;
    return c;
}

void decode_file() {
    FILE *in = fopen("Combined.txt", "r");
    FILE *out = fopen("Decoded.txt", "w");
    if (!in || !out) return;

    char c;
    while ((c = fgetc(in)) != EOF) fputc(rot13(c), out);

    fclose(in);
    fclose(out);
    printf("Decoded.txt created.\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        download_and_extract();
    } else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0) {
            filter_files();
        } else if (strcmp(argv[2], "Combine") == 0) {
            combine_files();
        } else if (strcmp(argv[2], "Decode") == 0) {
            decode_file();
        } else {
            fprintf(stderr, "Invalid mode. Usage: ./action [-m Filter|Combine|Decode]\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Usage: ./action [-m Filter|Combine|Decode]\n");
        return 1;
    }
    return 0;
}