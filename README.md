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
- `char *const argv[]`: mendeklarasikan parameter yang menerima array pointer (array yang berisi alamat string) dan menjamin bahwa elemen array (pointer) tidak akan diubah. Namun, string yang ditunjuk oleh pointer masih bisa dimodifikasi.
- `pid_t pid = fork();`: membuat proses anak, dimana fungsi fork() mengembalikan nilai 0 pada proses anak (di mana kode berikutnya akan dieksekusi oleh anak), nilai positif (PID dari proses anak) pada proses induk, dan nilai negatif jika fork() gagal.


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


## Soal 2

## Soal 3

## Soal 4

