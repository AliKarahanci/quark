/**
 * Quark Launcher
 * Maintainer: Ali Karahancı <karahanciali@hotmail.com>
 *
 * Original URL:
 * https://github.com/AliKarahanci/quark
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_PATH 4096

void print_usage() {
    printf("Kullanım:\n");
    printf("  quark --list wine                         : Yüklü Wine/Proton sürümlerini listeler.\n");
    printf("  quark --wine <versiyon> <exe> [argümanlar]: Belirtilen sürümle uygulamayı başlatır.\n");
    printf("\nÖrnek:\n");
    printf("  quark --wine proton-ge-8.0 game.exe\n");
}

void get_config_path(char *buffer) {
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
        fprintf(stderr, "Hata: HOME çevre değişkeni bulunamadı.\n");
        exit(1);
    }
    snprintf(buffer, MAX_PATH, "%s/.quarklauncher", home_dir);
}

void list_versions() {
    char config_path[MAX_PATH];
    get_config_path(config_path);

    DIR *d = opendir(config_path);
    if (!d) {
        if (mkdir(config_path, 0755) == -1) {
            if (errno == EEXIST) {
                perror("Dizin açılamadı");
            } else {
                printf("İlk çalıştırma: %s oluşturuldu. Lütfen içine Wine sürümlerini ekleyin.\n", config_path);
            }
        } else {
            printf("Klasör oluşturuldu: %s\nLütfen buraya Wine/Proton klasörlerinizi kopyalayın.\n", config_path);
        }
        return;
    }

    printf("Mevcut Wine/Proton Sürümleri (%s):\n", config_path);
    printf("--------------------------------------------------\n");

    struct dirent *dir;
    int count = 0;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] != '.') {
            printf("  - %s\n", dir->d_name);
            count++;
        }
    }

    if (count == 0) {
        printf("  (Hiçbir sürüm bulunamadı)\n");
    }

    printf("--------------------------------------------------\n");
    closedir(d);
}

void launch_app(char *version, int argc, char *argv[]) {
    char config_path[MAX_PATH];
    char wine_bin_path[MAX_PATH];

    get_config_path(config_path);

    snprintf(wine_bin_path, MAX_PATH, "%s/%s/bin/wine", config_path, version);

    if (access(wine_bin_path, X_OK) == -1) {
        fprintf(stderr, "Hata: Wine binary'si bulunamadı veya çalıştırılabilir değil.\n");
        fprintf(stderr, "Aranan yol: %s\n", wine_bin_path);
        fprintf(stderr, "İpucu: Klasör yapısının {surum}/bin/wine şeklinde olduğundan emin olun.\n");
        exit(1);
    }

    printf("Quark: Başlatılıyor...\n");
    printf("Sürüm: %s\n", version);
    printf("Binary: %s\n", wine_bin_path);

    int exec_argc = argc - 3;

    char **new_argv = malloc(sizeof(char *) * (exec_argc + 2));

    new_argv[0] = wine_bin_path;

    for (int i = 0; i < exec_argc; i++) {
        new_argv[i + 1] = argv[i + 3];
    }
    new_argv[exec_argc + 1] = NULL;

    execv(wine_bin_path, new_argv);

    perror("Hata: Wine başlatılamadı");
    free(new_argv);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "--list") == 0 && argc >= 3 && strcmp(argv[2], "wine") == 0) {
        list_versions();
        return 0;
    }

    if (strcmp(argv[1], "--wine") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Hata: Eksik argüman.\n");
            print_usage();
            return 1;
        }
        launch_app(argv[2], argc, argv);
        return 0;
    }

    print_usage();
    return 1;
}
