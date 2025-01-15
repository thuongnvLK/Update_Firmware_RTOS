#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define FILE_NAME_LEN 256

typedef struct {
    char name[FILE_NAME_LEN];      // File or directory name
    int level;                     // Directory level (used for recursion)
    int is_dir;                    // Check if it's a directory
    char full_path[1024];          // Full path
    size_t size;                   // File size
    unsigned char *content;        // File content
} DirectoryEntry;

// Function to check if the file has a specific extension
int has_extension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, extension) == 0);
}

// Function to check if the path is a directory
int is_directory(const char *path) {
    DWORD attribs = GetFileAttributes(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

// Function to read file content
void read_file_content(const char *filename, DirectoryEntry *entry) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Cannot open file: %s\n", filename);
        return;
    }

    // Find file size
    fseek(fp, 0, SEEK_END);
    entry->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory to read content
    entry->content = (unsigned char *)malloc(entry->size);
    if (entry->content == NULL) {
        printf("Memory allocation failed for file: %s\n", filename);
        fclose(fp);
        return;
    }

    // Read file content
    fread(entry->content, 1, entry->size, fp);
    fclose(fp);
}

// Convert two hex characters to a byte
unsigned char hex_to_byte(const char *hex) {
    unsigned char byte = 0;

    // Ensure that the input string has at least two characters
    if (hex[0] == '\0' || hex[1] == '\0') {
        printf("Error: Invalid hex input\n");
        return 0; // Return 0 for invalid input
    }

    for (int i = 0; i < 2; i++) {
        byte <<= 4; // Shift left to make space for the next digit
        if (hex[i] >= '0' && hex[i] <= '9') {
            byte |= hex[i] - '0'; // Convert '0'-'9' to 0-9
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            byte |= hex[i] - 'A' + 10; // Convert 'A'-'F' to 10-15
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            byte |= hex[i] - 'a' + 10; // Convert 'a'-'f' to 10-15
        } else {
            printf("Error: Invalid hex character '%c'\n", hex[i]);
            return 0; // Return 0 for invalid hex character
        }
    }

    return byte;
}

// Function to read the content of a .hex file
void read_hex_file(const char *filename, DirectoryEntry *entry) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open .hex file: %s\n", filename);
        return;
    }

    entry->content = NULL;
    entry->size = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != ':') {
            continue; // Skip lines that do not start with ':'
        }

        int length = hex_to_byte(&line[1]);
        int record_type = hex_to_byte(&line[7]);

        if (record_type == 0) { // Data record
            unsigned char *new_content = realloc(entry->content, entry->size + length);
            if (new_content == NULL) {
                printf("Failed to allocate memory for .hex file: %s\n", filename);
                free(entry->content);
                fclose(fp);
                return;
            }
            entry->content = new_content;

            for (int i = 0; i < length; i++) {
                unsigned char byte = hex_to_byte(&line[9 + i * 2]);
                entry->content[entry->size++] = byte;
            }
        }
    }

    fclose(fp);
}

// Recursive function to read all directories and files in the project
int read_directory(const char *dir_name, int level, DirectoryEntry *entries, int *entry_count, const char *extension) {
    WIN32_FIND_DATA find_data;
    HANDLE hFind;
    char search_path[1024];

    snprintf(search_path, sizeof(search_path), "%s\\*", dir_name);
    hFind = FindFirstFile(search_path, &find_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory: %s\n", dir_name);
        return *entry_count;
    }

    do {
        if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
            char path[1024];
            snprintf(path, sizeof(path), "%s\\%s", dir_name, find_data.cFileName);

            DirectoryEntry entry;
            strncpy(entry.name, find_data.cFileName, sizeof(entry.name));
            entry.level = level;
            entry.is_dir = is_directory(path);
            strncpy(entry.full_path, path, sizeof(entry.full_path));

            if (!entry.is_dir && has_extension(find_data.cFileName, extension)) {
                read_hex_file(path, &entry); // Read the hex file
                entries[(*entry_count)++] = entry;
            }

            if (entry.is_dir) {
                read_directory(path, level + 1, entries, entry_count, extension);
            }
        }
    } while (FindNextFile(hFind, &find_data) != 0);

    FindClose(hFind);
    return *entry_count;
}

// Function to write information to the output file
void write_output_file(const char *filename, DirectoryEntry *entry) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Cannot open output file: %s\n", filename);
        return;
    }

    fprintf(fp, "File: %s\nSize: %zu bytes\nContent:\n", entry->name, entry->size);
    for (size_t i = 0; i < entry->size; i++) {
        fprintf(fp, "%02X ", entry->content[i]);
        if ((i + 1) % 16 == 0) {
            fprintf(fp, "\n");
        }
    }
    if (entry->size % 16 != 0) {
        fprintf(fp, "\n");
    }

    fclose(fp);
}

// Function to list found files
void list_files(DirectoryEntry *entries, int entry_count) {
    printf("List of found files:\n");
    for (int i = 0; i < entry_count; i++) {
        printf("%d. %s (%s)\n", i + 1, entries[i].name, entries[i].full_path);
    }
}

// Function to process and write information to the output file
void process_files(DirectoryEntry *entries, int entry_count, const char *output_extension) {
    for (int i = 0; i < entry_count; i++) {
        char output_file_name[FILE_NAME_LEN];
        printf("Enter output name for file %s (without extension):\n", entries[i].name);
        scanf("%s", output_file_name);
        strcat(output_file_name, output_extension);  // Append extension

        // Write information to output file
        write_output_file(output_file_name, &entries[i]);

        // Free memory after writing file
        free(entries[i].content);
    }
    printf("Processed %d files.\n", entry_count);
}

int main() {
    DirectoryEntry entries[100]; // Assuming a maximum of 100 files
    int entry_count = 0;
    char extension[10], output_extension[10];

    // Ask user for file extension to search
    printf("Enter the file extension to search (e.g., .hex): ");
    scanf("%s", extension);

    // Ask user for output file extension
    printf("Enter the output file extension (e.g., .hala): ");
    scanf("%s", output_extension);

    // Read all files with the specified extension from root directory "."
    read_directory(".", 0, entries, &entry_count, extension);

    // List found files
    list_files(entries, entry_count);

    // Process and write output files
    process_files(entries, entry_count, output_extension);

    return 0;
}