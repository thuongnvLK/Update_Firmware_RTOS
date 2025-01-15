#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define HEX_FILE_NAME_LEN 256

typedef struct {
    char name[HEX_FILE_NAME_LEN];
    size_t size; // Kích thước dữ liệu
    unsigned char *content; // Dữ liệu của file .hex
} HexFile;

typedef struct {
    char name[256];
    int level;
    int is_dir;
    char full_path[1024];
} DirectoryEntry;

// Hàm chuyển đổi 2 ký tự hexa thành một byte
unsigned char hex_to_byte(const char *hex) {
    unsigned char byte = 0;
    for (int i = 0; i < 2; i++) {
        byte <<= 4;
        if (hex[i] >= '0' && hex[i] <= '9') {
            byte |= hex[i] - '0';
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            byte |= hex[i] - 'A' + 10;
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            byte |= hex[i] - 'a' + 10;
        }
    }
    return byte;
}

// Hàm đọc nội dung file .hex
void read_hex_file(const char *filename, HexFile *hex_file) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open file: %s\n", filename);
        return;
    }

    hex_file->size = 0;
    hex_file->content = NULL;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != ':') {
            continue;
        }

        int length = hex_to_byte(&line[1]);
        int record_type = hex_to_byte(&line[7]);

        if (record_type == 0) {
            unsigned char *new_content = realloc(hex_file->content, hex_file->size + length);
            if (new_content == NULL) {
                printf("Failed to allocate memory for file: %s\n", filename);
                free(hex_file->content);
                fclose(fp);
                return;
            }
            hex_file->content = new_content;

            for (int i = 0; i < length; i++) {
                unsigned char byte = hex_to_byte(&line[9 + i * 2]);
                hex_file->content[hex_file->size++] = byte;
            }
        }
    }

    fclose(fp);
}

int is_directory(const char *path) {
    DWORD attribs = GetFileAttributes(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

int read_directory(const char *dir_name, int level, DirectoryEntry *entries, int entry_count) {
    WIN32_FIND_DATA find_data;
    HANDLE hFind;
    char search_path[1024];

    snprintf(search_path, sizeof(search_path), "%s\\*", dir_name);
    hFind = FindFirstFile(search_path, &find_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory: %s\n", dir_name);
        return entry_count;
    }

    do {
        if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
            char path[1024];
            snprintf(path, sizeof(path), "%s\\%s", dir_name, find_data.cFileName);

            strcpy(entries[entry_count].name, find_data.cFileName);
            entries[entry_count].level = level;
            entries[entry_count].is_dir = is_directory(path);
            strcpy(entries[entry_count].full_path, path);
            entry_count++;

            if (entries[entry_count - 1].is_dir) {
                entry_count = read_directory(path, level + 1, entries, entry_count);
            }
        }
    } while (FindNextFile(hFind, &find_data) != 0);

    FindClose(hFind);
    return entry_count;
}

int has_hex_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, ".hex") == 0);
}


void hex_list(DirectoryEntry *entries, int entry_count) {
    printf("--Danh sach file.hex trong Project--\n");
    for (int i = 0; i < entry_count; i++) {
        if (!entries[i].is_dir && has_hex_extension(entries[i].name)) {
            printf("%s and %s\n", entries[i].name, entries[i].full_path);
        }
    }
}

int main() {
    int entry_count = 0;
    DirectoryEntry entries[100];

    entry_count = read_directory(".", 0, entries, entry_count);

    hex_list(entries, entry_count);
    return 0;
}
