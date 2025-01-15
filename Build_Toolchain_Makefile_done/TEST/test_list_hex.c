#include <windows.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"

typedef struct {
    char name[256]; // Tên file or thực mục
    int level; // Cấp của thự mục
    int is_dir; 
    char full_path[1024];
} DirectoryEntry;

int is_direction(const char *path) { // kiểm tra có phải thưc mục hay không
    DWORD ATTRIBS = GetFileAttributes(path);
    if (ATTRIBS == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    return (ATTRIBS & FILE_ATTRIBUTE_DIRECTORY);
}

int has_hex(const char *fileName) {
    const char* dot = strrchr(fileName, '.');
    return (dot && strcmp(dot, ".hex") == 0);
}
int Read_Direction(const char *dir_name, int level, DirectoryEntry *Entries, int *entry_count) {
    WIN32_FIND_DATA findData; //
    HANDLE hFind;
    char search[1024];

    sprintf(search, "%s\\*", dir_name);
    hFind = FindFirstFile(search, &findData);

    do {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            char path_file[1024];
            sprintf(path_file, "%s\\%s", dir_name, findData.cFileName);
            DirectoryEntry entry;
            strncpy(entry.name, findData.cFileName, sizeof(entry.name));
            entry.level = level;
            entry.is_dir = is_direction(path_file);
            strncpy(entry.full_path, path_file, sizeof(entry.full_path));

            if (!entry.is_dir && has_hex(findData.cFileName)) {
                Entries[(*entry_count)++] = entry;
            }

            if (entry.is_dir) { // 
                Read_Direction(path_file, level + 1, Entries, entry_count); 
            }
        }

    }while(FindNextFile(hFind, &findData));

    FindClose(hFind);
    return *entry_count;

    // FindFirstFile() ~ open file 
    // FindNextFile() ~ read
    // 
}

int main() {
    DirectoryEntry entries[256];
    int entry_count = 0;
    Read_Direction(".", 0, entries, &entry_count);

    printf("List hex: \n");
    for (int i=0; i < entry_count; i++) {
        printf("%d. %s - %s\n",i, entries[i].name, entries[i].full_path);
    }
}

