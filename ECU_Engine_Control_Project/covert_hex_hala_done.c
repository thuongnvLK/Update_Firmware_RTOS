#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 256
#define FILE_NAME_LEN 256
#define MAX_ENTRIES 100

typedef struct {
    char name[FILE_NAME_LEN];      // Tên tệp hoặc thư mục
    int level;                     // Cấp độ thư mục cho đệ quy
    int is_dir;                    // 1 nếu là thư mục, 0 nếu không
    char full_path[1024];          // Đường dẫn đầy đủ của tệp hoặc thư mục
    size_t size;                   // Kích thước của tệp
    size_t data_size;              // Kích thước dữ liệu (không bao gồm checksum)
    unsigned char *content;        // Nội dung của tệp
    unsigned long address;         // Địa chỉ RAM
} DirectoryEntry;

/**
 * @brief Chuyển đổi hai ký tự hex thành một byte.
 *
 * @param hex Chuỗi chứa hai ký tự hex
 * @return Giá trị byte tương ứng
 */
unsigned char hex_to_byte(const char *hex) {
    unsigned char byte = 0;

    // Đảm bảo đầu vào hex hợp lệ
    if (hex[0] == '\0' || hex[1] == '\0') {
        printf("Error: Invalid hex input\n");
        return 0;
    }

    for (int i = 0; i < 2; i++) {
        byte <<= 4; // Dịch trái
        if (hex[i] >= '0' && hex[i] <= '9') {
            byte |= hex[i] - '0';
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            byte |= hex[i] - 'A' + 10;
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            byte |= hex[i] - 'a' + 10;
        } else {
            printf("Error: Invalid hex character '%c'\n", hex[i]);
            return 0;
        }
    }

    return byte;
}

/**
 * @brief Tính checksum cho nội dung của tệp.
 *
 * @param data Con trỏ đến nội dung tệp
 * @param length Kích thước của nội dung
 * @return Giá trị checksum
 */
unsigned char calculate_checksum(const unsigned char *data, size_t length) {
    unsigned long sum = 0; // Sử dụng long để tránh tràn số trong quá trình cộng

    // Cộng tất cả các byte trong mảng data
    for (size_t i = 0; i < length; i++) {
        sum += data[i];
    }

    // Thực hiện AND với 0xFF để chỉ lấy 8 bit thấp nhất của tổng
    return (unsigned char)(sum & 0xFF);
}

/**
 * @brief Kiểm tra xem một tệp có đuôi cụ thể hay không.
 *
 * @param filename Tên tệp
 * @param extension Đuôi cần kiểm tra
 * @return 1 nếu có đuôi, 0 nếu không
 */
int has_extension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, extension) == 0);
}

/**
 * @brief Kiểm tra xem đường dẫn có phải là thư mục không.
 *
 * @param path Đường dẫn cần kiểm tra
 * @return 1 nếu là thư mục, 0 nếu không
 */
int is_directory(const char *path) {
    DWORD attribs = GetFileAttributes(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

/**
 * @brief Đọc nội dung của một tệp.
 *
 * @param filename Tên tệp cần đọc
 * @param entry Cấu trúc DirectoryEntry để lưu dữ liệu tệp
 */
typedef enum {
    START_CODE  =   0U,
    BYTE_COUNT  =   1U,
    ADDRESS     =   2U,
    RECORD_TYPE =   3U,
    DATA        =   4U,
    CHECKSUM    =   5U,
    CAlCULATE   =   6U
}FieldHexFile;

typedef enum {
    READ_SUCCESSFULLY = 0U,
    READ_FAILED
}StatusReadHexFile;

typedef struct {
    // struct
    // {
        uint8_t u8ByteCount;
        uint8_t u8Addr[2];
        uint8_t u8RecordType;
        uint8_t u8Data[16];
        uint8_t u8CheckSum;
    // };

} HexFormData;

int ReadLine(FILE *fp, char *buffer);
void ConvertStringToHex(char *buff, int u32Length);
StatusReadHexFile ReadAllLine(FILE *fp, char *pBufferData);
char cBufferData[50];
char iInternalBufferData[50];

HexFormData hexStruct;
FILE *fp;
int i = 0;

int ReadLine(FILE *fp, char *pBufferData) {
    int u32Count = 0U;
    char cDataLine;

    while ((cDataLine = fgetc(fp)) != '\n')
    {
        pBufferData[u32Count] = cDataLine;
        u32Count++;
    }
    
    if (cDataLine == NULL)
    {
        return 0;
    }
    
    return 1;

}

void ConvertStringToHex(char *buff, int u32Length) {
    int u32Count = 4U;
    int u32Count1 = 0U;
    int tempBuffer[100];

    for (u32Count = 0U; u32Count < u32Length; u32Count++) {
        if (buff[u32Count] >= '0' && buff[u32Count] <= '9') {
            tempBuffer[u32Count] = buff[u32Count] - 48;
        } else if (buff[u32Count] >= 'A' && buff[u32Count] <= 'F') {
            tempBuffer[u32Count] = buff[u32Count] - 65 + 10;
        } else {
            return;
        }

        if ((u32Count %2) != 0) {
        iInternalBufferData[u32Count1] = ((tempBuffer[u32Count - 1] << 4) | (tempBuffer[u32Count]));
        u32Count1++;
        }
    } 
}

void read_file_content(const char *filename, DirectoryEntry *entry) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Cannot open file: %s\n", filename);
        return;
    }

    // Tìm kích thước tệp
    fseek(fp, 0, SEEK_END);
    entry->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Cấp phát bộ nhớ cho nội dung
    entry->content = (unsigned char *)malloc(entry->size);
    if (entry->content == NULL) {
        printf("Memory allocation failed for file: %s\n", filename);
        fclose(fp);
        return;
    }

    // Đọc nội dung tệp
    fread(entry->content, 1, entry->size, fp);
    fclose(fp);
}


/**
 * @brief Đọc nội dung của một tệp .hex.
 *
 * @param filename Tên tệp .hex cần đọc
 * @param entry Cấu trúc DirectoryEntry để lưu dữ liệu tệp
 */
void read_hex_file(const char *filename, DirectoryEntry *entry) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open .hex file: %s\n", filename);
        return;
    }

    entry->content = NULL;
    entry->size = 0;
    entry->data_size = 0; // Kích thước không bao gồm checksum
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != ':') {
            continue; // Bỏ qua các dòng không bắt đầu bằng ':'
        }

        int length = hex_to_byte(&line[1]);
        int record_type = hex_to_byte(&line[7]);

        if (record_type == 0) { // Dữ liệu
            unsigned char *new_content = realloc(entry->content, entry->size + length + 1);
            if (new_content == NULL) {
                printf("Failed to allocate memory for .hex file: %s\n", filename);
                free(entry->content);
                fclose(fp);
                return;
            }
            entry->content = new_content;

             // Đọc nội dung dữ liệu
            for (int i = 0; i < length; i++) {
                unsigned char byte = hex_to_byte(&line[9 + i * 2]);
                entry->content[entry->size++] = byte;
                entry->data_size++; // Tăng kích thước dữ liệu
            }

            // Tính toán checksum cho 8 byte đầu tiên
            unsigned char checksum = calculate_checksum(entry->content + entry->size - length, length);
            entry->content[entry->size++] = checksum; // Thêm checksum vào cuối
        }
    }

    fclose(fp);
}
// void read_hex_file(const char *filename, DirectoryEntry *entry) {
//     FILE *fp = fopen(filename, "r");
//     if (!fp) {
//         printf("Cannot open .hex file: %s\n", filename);
//         return;
//     }

//     entry->content = NULL;
//     entry->size = 0;
//     entry->data_size = 0;  // Kích thước không bao gồm checksum
//     uint8_t u8CalculCheckSum;
//     FieldHexFile State;
//     int u32Count = 0;
//     int u32DataCount = 0;
//     int cntLine = 0;

//     while (ReadLine(fp, cBufferData)) {
//         for (u32DataCount = 0; u32DataCount < 16; u32DataCount++) {
//             hexStruct.u8Data[u32DataCount] = 0;
//         }

//         cntLine++;
//         State = START_CODE;
//         for (u32Count = 0; u32Count < strlen(cBufferData); u32Count++) {
//             switch (State) {
//                 case START_CODE:
//                     if (cBufferData[u32Count] != ':') {
//                         printf("Invalid start code in line %d\n", cntLine);
//                         fclose(fp);
//                         return;
//                     }
//                     State = BYTE_COUNT;
//                     break;

//                 case BYTE_COUNT:
//                     ConvertStringToHex(&cBufferData[u32Count], 2);
//                     hexStruct.u8ByteCount = iInternalBufferData[0];
//                     u32Count += 1;  // Nhảy qua byte count
//                     State = ADDRESS;
//                     break;

//                 case ADDRESS:
//                     ConvertStringToHex(&cBufferData[u32Count], 4);
//                     hexStruct.u8Addr[0] = iInternalBufferData[0];
//                     hexStruct.u8Addr[1] = iInternalBufferData[1];
//                     u32Count += 3;  // Nhảy qua địa chỉ
//                     State = RECORD_TYPE;
//                     break;

//                 case RECORD_TYPE:
//                     ConvertStringToHex(&cBufferData[u32Count], 2);
//                     hexStruct.u8RecordType = iInternalBufferData[0];
//                     u32Count += 1;  // Nhảy qua record type
//                     State = DATA;
//                     break;

//                 case DATA:
//                     ConvertStringToHex(&cBufferData[u32Count], (hexStruct.u8ByteCount * 2));
//                     unsigned char *new_content = realloc(entry->content, entry->size + hexStruct.u8ByteCount);
//                     if (new_content == NULL) {
//                         printf("Failed to allocate memory\n");
//                         free(entry->content);
//                         fclose(fp);
//                         return;
//                     }
//                     entry->content = new_content;

//                     for (u32DataCount = 0; u32DataCount < hexStruct.u8ByteCount; u32DataCount++) {
//                         entry->content[entry->size++] = iInternalBufferData[u32DataCount];
//                         entry->data_size++;
//                     }

//                     u32Count += (hexStruct.u8ByteCount * 2 - 1);  // Nhảy qua dữ liệu
//                     State = CHECKSUM;
//                     break;

//                 case CHECKSUM:
//                     ConvertStringToHex(&cBufferData[u32Count], 2);
//                     hexStruct.u8CheckSum = iInternalBufferData[0];
//                     State = CAlCULATE;
//                     break;

//                 case CAlCULATE:
//                     u8CalculCheckSum = hexStruct.u8ByteCount + hexStruct.u8Addr[0] + hexStruct.u8Addr[1] + hexStruct.u8RecordType;
//                     for (u32DataCount = 0; u32DataCount < hexStruct.u8ByteCount; u32DataCount++) {
//                         u8CalculCheckSum += hexStruct.u8Data[u32DataCount];
//                     }
//                     u8CalculCheckSum = ~u8CalculCheckSum + 1;

//                     if (u8CalculCheckSum != hexStruct.u8CheckSum) {
//                         printf("Checksum mismatch in line %d\n", cntLine);
//                         fclose(fp);
//                         return;
//                     }

//                     if (hexStruct.u8RecordType == 0x01) {  // End of file record
//                         fclose(fp);
//                         return;
//                     }

//                     break;

//                 default:
//                     printf("Invalid state in line %d\n", cntLine);
//                     fclose(fp);
//                     return;
//             }
//         }
//     }

//     fclose(fp);
// }


/**
 * @brief Đệ quy đọc tất cả thư mục và tệp trong dự án.
 *
 * @param dir_name Tên thư mục cần đọc
 * @param level Cấp độ thư mục hiện tại
 * @param entries Mảng để lưu trữ cấu trúc DirectoryEntry
 * @param entry_count Con trỏ đến số lượng mục
 * @param extension Đuôi tệp để tìm kiếm
 * @return Tổng số mục tìm thấy
 */
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
                read_hex_file(path, &entry); // Đọc tệp hex
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

/**
 * @brief Ghi thông tin vào tệp đầu ra.
 *
 * @param filename Tên tệp đầu ra
 * @param entry Cấu trúc DirectoryEntry chứa dữ liệu tệp
 */
void write_output_file(const char *filename, DirectoryEntry *entry) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Cannot open output file: %s\n", filename);
        return;
    }

    // fprintf(fp, "File: %s\nSize: %zu bytes\nContent:\n", entry->name, entry->data_size);
    fprintf(fp, "RAM Address: 0x%08lX\nFile: %s\nData Size (without checksum): %zu bytes\nContent:\n", 
            entry->address, entry->name, entry->data_size);
    for (size_t i = 0; i < entry->size; i++) {
        fprintf(fp, "%02X ", entry->content[i]);
        if ((i + 1) % 17 == 0) {
            fprintf(fp, "\n");
        }
    }
    if (entry->size % 17 != 0) {
        fprintf(fp, "\n");
    }

    fclose(fp);
}

/**
 * @brief Liệt kê các tệp đã tìm thấy trên console.
 *
 * @param entries Mảng các cấu trúc DirectoryEntry
 * @param entry_count Tổng số mục tìm thấy
 */
void list_files(DirectoryEntry *entries, int entry_count) {
    printf("List of found files:\n");
    for (int i = 0; i < entry_count; i++) {
        printf("%d. %s (%s)\n", i + 1, entries[i].name, entries[i].full_path);
    }
}

/**
 * @brief Xử lý và ghi thông tin vào các tệp đầu ra.
 *
 * @param entries Mảng các cấu trúc DirectoryEntry
 * @param entry_count Tổng số mục tìm thấy
 * @param output_extension Đuôi tệp cho các tệp đầu ra
 */
// void process_files(DirectoryEntry *entries, int entry_count, const char *output_extension) {
//     for (int i = 0; i < entry_count; i++) {
//         char output_file_name[FILE_NAME_LEN];
//         printf("Enter output name for file %s (without extension):\n", entries[i].name);
//         scanf("%s", output_file_name);
//         strcat(output_file_name, output_extension);  // Thêm đuôi

//         // Nhập địa chỉ RAM từ người dùng
//         printf("Enter RAM address for file %s (in hex, e.g., 0x20000000):\n", entries[i].name);
//         scanf("%lx", &entries[i].address);  // Nhập địa chỉ RAM

//         // Ghi thông tin vào tệp đầu ra
//         write_output_file(output_file_name, &entries[i]);

//         // Giải phóng bộ nhớ sau khi ghi tệp
//         free(entries[i].content);
//     }
//     printf("Processed %d files.\n", entry_count);
// }
void process_file(DirectoryEntry *entry, const char *output_extension) {
    char output_file_name[FILE_NAME_LEN];
    printf("Enter output name for file %s (without extension):\n", entry->name);
    scanf("%s", output_file_name);
    strcat(output_file_name, output_extension);  // Thêm đuôi

    // Nhập địa chỉ RAM từ người dùng
    printf("Enter RAM address for file %s (in hex, e.g., 0x20000000):\n", entry->name);
    scanf("%lx", &entry->address);  // Nhập địa chỉ RAM

    // Ghi thông tin vào tệp đầu ra
    write_output_file(output_file_name, entry);

    // Giải phóng bộ nhớ sau khi ghi tệp
    free(entry->content);
}
/**
 * @brief Hàm chính để khởi động chương trình.
 *
 * @return Trạng thái thoát
 */
int main() {
    DirectoryEntry entries[MAX_ENTRIES]; // Mảng để chứa các mục tệp
    int entry_count = 0;
    char extension[10], output_extension[10]; // Phần mở rộng file input/output

    // Yêu cầu người dùng nhập đuôi tệp cần tìm
    printf("Enter the file extension to search (e.g., .hex): ");
    scanf("%s", extension);

    // Yêu cầu người dùng nhập đuôi tệp đầu ra
    printf("Enter the output file extension (e.g., .hala): ");
    scanf("%s", output_extension);

    // Đọc tất cả các tệp với đuôi đã chỉ định từ thư mục gốc
    read_directory(".", 0, entries, &entry_count, extension);

    // Liệt kê các tệp đã tìm thấy
    list_files(entries, entry_count);

    // Xử lý và ghi các tệp đầu ra
    // process_files(entries, entry_count, output_extension);
    int file_index;
    printf("Enter the index of the file to process (1 to %d): ", entry_count);
    scanf("%d", &file_index);
    
    if (file_index < 1 || file_index > entry_count) {
        printf("Invalid file index.\n");
    } else {
        process_file(&entries[file_index - 1], output_extension); // Chọn tệp để xử lý
    }
    return 0;
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <windows.h>
// #include <string.h>

// #define MAX_LINE_LENGTH 256
// #define FILE_NAME_LEN 256

// typedef struct {
//     char name[FILE_NAME_LEN];      // File or directory name
//     int level;                     // Directory level (used for recursion)
//     int is_dir;                    // Check if it's a directory
//     char full_path[1024];          // Full path
//     size_t size;                   // File size
//     unsigned char *content;        // File content
// } DirectoryEntry;

// // Function to check if the file has a specific extension
// int has_extension(const char *filename, const char *extension) {
//     const char *dot = strrchr(filename, '.');
//     return (dot && strcmp(dot, extension) == 0);
// }

// // Function to check if the path is a directory
// int is_directory(const char *path) {
//     DWORD attribs = GetFileAttributes(path);
//     if (attribs == INVALID_FILE_ATTRIBUTES) {
//         return 0;
//     }
//     return (attribs & FILE_ATTRIBUTE_DIRECTORY);
// }

// // Function to read file content
// void read_file_content(const char *filename, DirectoryEntry *entry) {
//     FILE *fp = fopen(filename, "rb");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     // Find file size
//     fseek(fp, 0, SEEK_END);
//     entry->size = ftell(fp);
//     fseek(fp, 0, SEEK_SET);

//     // Allocate memory to read content
//     entry->content = (unsigned char *)malloc(entry->size);
//     if (entry->content == NULL) {
//         printf("Memory allocation failed for file: %s\n", filename);
//         fclose(fp);
//         return;
//     }

//     // Read file content
//     fread(entry->content, 1, entry->size, fp);
//     fclose(fp);
// }

// // Convert two hex characters to a byte
// unsigned char hex_to_byte(const char *hex) {
//     unsigned char byte = 0;

//     // Ensure that the input string has at least two characters
//     if (hex[0] == '\0' || hex[1] == '\0') {
//         printf("Error: Invalid hex input\n");
//         return 0; // Return 0 for invalid input
//     }

//     for (int i = 0; i < 2; i++) {
//         byte <<= 4; // Shift left to make space for the next digit
//         if (hex[i] >= '0' && hex[i] <= '9') {
//             byte |= hex[i] - '0'; // Convert '0'-'9' to 0-9
//         } else if (hex[i] >= 'A' && hex[i] <= 'F') {
//             byte |= hex[i] - 'A' + 10; // Convert 'A'-'F' to 10-15
//         } else if (hex[i] >= 'a' && hex[i] <= 'f') {
//             byte |= hex[i] - 'a' + 10; // Convert 'a'-'f' to 10-15
//         } else {
//             printf("Error: Invalid hex character '%c'\n", hex[i]);
//             return 0; // Return 0 for invalid hex character
//         }
//     }

//     return byte;
// }

// // Function to read the content of a .hex file
// void read_hex_file(const char *filename, DirectoryEntry *entry) {
//     FILE *fp = fopen(filename, "r");
//     if (!fp) {
//         printf("Cannot open .hex file: %s\n", filename);
//         return;
//     }

//     entry->content = NULL;
//     entry->size = 0;
//     char line[MAX_LINE_LENGTH];

//     while (fgets(line, sizeof(line), fp)) {
//         if (line[0] != ':') {
//             continue; // Skip lines that do not start with ':'
//         }

//         int length = hex_to_byte(&line[1]);
//         int record_type = hex_to_byte(&line[7]);

//         if (record_type == 0) { // Data record
//             unsigned char *new_content = realloc(entry->content, entry->size + length);
//             if (new_content == NULL) {
//                 printf("Failed to allocate memory for .hex file: %s\n", filename);
//                 free(entry->content);
//                 fclose(fp);
//                 return;
//             }
//             entry->content = new_content;

//             for (int i = 0; i < length; i++) {
//                 unsigned char byte = hex_to_byte(&line[9 + i * 2]);
//                 entry->content[entry->size++] = byte;
//             }
//         }
//     }

//     fclose(fp);
// }

// // Recursive function to read all directories and files in the project
// int read_directory(const char *dir_name, int level, DirectoryEntry *entries, int *entry_count, const char *extension) {
//     WIN32_FIND_DATA find_data;
//     HANDLE hFind;
//     char search_path[1024];

//     snprintf(search_path, sizeof(search_path), "%s\\*", dir_name);
//     hFind = FindFirstFile(search_path, &find_data);

//     if (hFind == INVALID_HANDLE_VALUE) {
//         printf("Error opening directory: %s\n", dir_name);
//         return *entry_count;
//     }

//     do {
//         if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
//             char path[1024];
//             snprintf(path, sizeof(path), "%s\\%s", dir_name, find_data.cFileName);

//             DirectoryEntry entry;
//             strncpy(entry.name, find_data.cFileName, sizeof(entry.name));
//             entry.level = level;
//             entry.is_dir = is_directory(path);
//             strncpy(entry.full_path, path, sizeof(entry.full_path));

//             if (!entry.is_dir && has_extension(find_data.cFileName, extension)) {
//                 read_hex_file(path, &entry); // Read the hex file
//                 entries[(*entry_count)++] = entry;
//             }

//             if (entry.is_dir) {
//                 read_directory(path, level + 1, entries, entry_count, extension);
//             }
//         }
//     } while (FindNextFile(hFind, &find_data) != 0);

//     FindClose(hFind);
//     return *entry_count;
// }

// // Function to write information to the output file
// void write_output_file(const char *filename, DirectoryEntry *entry) {
//     FILE *fp = fopen(filename, "w");
//     if (!fp) {
//         printf("Cannot open output file: %s\n", filename);
//         return;
//     }

//     fprintf(fp, "File: %s\nSize: %zu bytes\nContent:\n", entry->name, entry->size);
//     for (size_t i = 0; i < entry->size; i++) {
//         fprintf(fp, "%02X ", entry->content[i]);
//         if ((i + 1) % 16 == 0) {
//             fprintf(fp, "\n");
//         }
//     }
//     if (entry->size % 16 != 0) {
//         fprintf(fp, "\n");
//     }

//     fclose(fp);
// }

// // Function to list found files
// void list_files(DirectoryEntry *entries, int entry_count) {
//     printf("List of found files:\n");
//     for (int i = 0; i < entry_count; i++) {
//         printf("%d. %s (%s)\n", i + 1, entries[i].name, entries[i].full_path);
//     }
// }

// // Function to process and write information to the output file
// void process_files(DirectoryEntry *entries, int entry_count, const char *output_extension) {
//     for (int i = 0; i < entry_count; i++) {
//         char output_file_name[FILE_NAME_LEN];
//         printf("Enter output name for file %s (without extension):\n", entries[i].name);
//         scanf("%s", output_file_name);
//         strcat(output_file_name, output_extension);  // Append extension

//         // Write information to output file
//         write_output_file(output_file_name, &entries[i]);

//         // Free memory after writing file
//         free(entries[i].content);
//     }
//     printf("Processed %d files.\n", entry_count);
// }

// int main() {
//     DirectoryEntry entries[100]; // Assuming a maximum of 100 files
//     int entry_count = 0;
//     char extension[10], output_extension[10];

//     // Ask user for file extension to search
//     printf("Enter the file extension to search (e.g., .hex): ");
//     scanf("%s", extension);

//     // Ask user for output file extension
//     printf("Enter the output file extension (e.g., .hala): ");
//     scanf("%s", output_extension);

//     // Read all files with the specified extension from root directory "."
//     read_directory(".", 0, entries, &entry_count, extension);

//     // List found files
//     list_files(entries, entry_count);

//     // Process and write output files
//     process_files(entries, entry_count, output_extension);

//     return 0;
// }







// #include <stdio.h>
// #include <stdlib.h>
// #include <windows.h>
// #include <string.h>

// #define MAX_LINE_LENGTH 256
// #define HEX_FILE_NAME_LEN 256

// typedef struct {
//     char name[HEX_FILE_NAME_LEN];
//     size_t size; // Kích thước dữ liệu
//     unsigned char *content; // Dữ liệu của file .hex
// } HexFile;

// typedef struct {
//     char name[256];
//     int level;
//     int is_dir;
//     char full_path[1024];
// } DirectoryEntry;

// // Hàm chuyển đổi 2 ký tự hexa thành một byte
// unsigned char hex_to_byte(const char *hex) {
//     unsigned char byte = 0;
//     for (int i = 0; i < 2; i++) {
//         byte <<= 4;
//         if (hex[i] >= '0' && hex[i] <= '9') {
//             byte |= hex[i] - '0';
//         } else if (hex[i] >= 'A' && hex[i] <= 'F') {
//             byte |= hex[i] - 'A' + 10;
//         } else if (hex[i] >= 'a' && hex[i] <= 'f') {
//             byte |= hex[i] - 'a' + 10;
//         }
//     }
//     return byte;
// }

// // Hàm đọc nội dung file .hex
// void read_hex_file(const char *filename, HexFile *hex_file) {
//     FILE *fp = fopen(filename, "r");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     hex_file->size = 0;
//     hex_file->content = NULL;
//     char line[MAX_LINE_LENGTH];

//     while (fgets(line, sizeof(line), fp)) {
//         if (line[0] != ':') {
//             continue;
//         }

//         int length = hex_to_byte(&line[1]);
//         int record_type = hex_to_byte(&line[7]);

//         if (record_type == 0) {
//             unsigned char *new_content = realloc(hex_file->content, hex_file->size + length);
//             if (new_content == NULL) {
//                 printf("Failed to allocate memory for file: %s\n", filename);
//                 free(hex_file->content);
//                 fclose(fp);
//                 return;
//             }
//             hex_file->content = new_content;

//             for (int i = 0; i < length; i++) {
//                 unsigned char byte = hex_to_byte(&line[9 + i * 2]);
//                 hex_file->content[hex_file->size++] = byte;
//             }
//         }
//     }

//     fclose(fp);
// }

// // Hàm kiểm tra xem file có phần mở rộng .hex không
// int has_hex_extension(const char *filename) {
//     const char *dot = strrchr(filename, '.');
//     return (dot && strcmp(dot, ".hex") == 0);
// }


// void hex_list(DirectoryEntry *entries, int entry_count) {
//     printf("--Danh sach file.hex trong Project--\n");
//     for (int i = 0; i < entry_count; i++) {
//         if (!entries[i].is_dir && has_hex_extension(entries[i].name)) {
//             printf("%s and %s\n", entries[i].name, entries[i].full_path);
//         }
//     }
// }

// // Hàm kiểm tra xem thư mục có phải là thư mục hay không
// int is_directory(const char *path) {
//     DWORD attribs = GetFileAttributes(path);
//     if (attribs == INVALID_FILE_ATTRIBUTES) {
//         return 0;
//     }
//     return (attribs & FILE_ATTRIBUTE_DIRECTORY);
// }

// // Hàm đệ quy để duyệt tất cả các thư mục trong project
// int read_directory(const char *dir_name, int level, HexFile *hex_files, int *file_count) {
//     WIN32_FIND_DATA find_data;
//     HANDLE hFind;
//     char search_path[1024];

//     snprintf(search_path, sizeof(search_path), "%s\\*", dir_name);
//     hFind = FindFirstFile(search_path, &find_data);

//     if (hFind == INVALID_HANDLE_VALUE) {
//         printf("Error opening directory: %s\n", dir_name);
//         return *file_count;
//     }

//     do {
//         if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
//             char path[1024];
//             snprintf(path, sizeof(path), "%s\\%s", dir_name, find_data.cFileName);

//             if (has_hex_extension(find_data.cFileName)) {
//                 HexFile hex_file;
//                 snprintf(hex_file.name, sizeof(hex_file.name), "%s\\%s", dir_name, find_data.cFileName);
//                 read_hex_file(hex_file.name, &hex_file);
//                 hex_files[(*file_count)++] = hex_file;
//             }

//             if (is_directory(path)) {
//                 read_directory(path, level + 1, hex_files, file_count);
//             }
//         }
//     } while (FindNextFile(hFind, &find_data) != 0);

//     FindClose(hFind);
//     return *file_count;
// }

// // Hàm ghi thông tin vào file .hala
// void write_hala_file(const char *filename, HexFile *hex_file) {
//     FILE *fp = fopen(filename, "w");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     fprintf(fp, "File: %s\nSize: %zu bytes\nContent:\n", hex_file->name, hex_file->size);
//     for (size_t j = 0; j < hex_file->size; j++) {
//         fprintf(fp, "%02X ", hex_file->content[j]);
//         if ((j + 1) % 16 == 0) {
//             fprintf(fp, "\n");
//         }
//     }
//     if (hex_file->size % 16 != 0) {
//         fprintf(fp, "\n");
//     }
//     fprintf(fp, "\n");

//     fclose(fp);
// }

// // Hàm liệt kê các file .hex
// void list_hex_files(HexFile *hex_files, int file_count) {
//     printf("Danh sach cac file .hex tim thay:\n");
//     for (int i = 0; i < file_count; i++) {
//         printf("%d. %s\n", i + 1, hex_files[i].name);
//     }
// }

// // Hàm yêu cầu người dùng nhập tên file .hala và ghi thông tin vào file
// void process_hex_files(HexFile *hex_files, int file_count) {
//     for (int i = 0; i < file_count; i++) {
//         char hala_file_name[HEX_FILE_NAME_LEN];
//         printf("Nhap ten file .hala cho file %s (khong bao gom phan mo rong):\n", hex_files[i].name);
//         scanf("%s", hala_file_name);
//         strcat(hala_file_name, ".hala"); // Thêm phần mở rộng .hala

//         // Ghi thông tin vào file .hala
//         write_hala_file(hala_file_name, &hex_files[i]);

//         // Giải phóng bộ nhớ sau khi ghi file
//         free(hex_files[i].content);
//     }
//     printf("Processed %d .hex files.\n", file_count);
// }

// int main() {
//     HexFile hex_files[100]; // Giả sử có thể có tối đa 100 file
//     int file_count = 0;

//     // Đọc tất cả các file .hex trong toàn bộ project (từ thư mục gốc ".")
//     read_directory(".", 0, hex_files, &file_count);

//     // Liệt kê các file .hex tìm thấy
//     list_hex_files(hex_files, file_count);

//     // Xử lý các file .hex và ghi ra file .hala
//     process_hex_files(hex_files, file_count);
//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <windows.h>
// #include <string.h>

// #define MAX_LINE_LENGTH 256
// #define HEX_FILE_NAME_LEN 256
// #define APPLICATION_FOLDER "Application" // Thư mục chứa file .hex

// typedef struct {
//     char name[HEX_FILE_NAME_LEN];
//     size_t size; // Kích thước dữ liệu
//     unsigned char *content; // Dữ liệu của file .hex, không cần kích thước cố định
// } HexFile;

// // Hàm chuyển đổi 2 ký tự hexa thành một byte
// unsigned char hex_to_byte(const char *hex) {
//     unsigned char byte = 0;
//     for (int i = 0; i < 2; i++) {
//         byte <<= 4;
//         if (hex[i] >= '0' && hex[i] <= '9') {
//             byte |= hex[i] - '0';
//         } else if (hex[i] >= 'A' && hex[i] <= 'F') {
//             byte |= hex[i] - 'A' + 10;
//         } else if (hex[i] >= 'a' && hex[i] <= 'f') {
//             byte |= hex[i] - 'a' + 10;
//         }
//     }
//     return byte;
// }

// // Hàm đọc nội dung file .hex
// void read_hex_file(const char *filename, HexFile *hex_file) {
//     FILE *fp = fopen(filename, "r");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     hex_file->size = 0; // Khởi tạo kích thước
//     hex_file->content = NULL; // Khởi tạo con trỏ nội dung
//     char line[MAX_LINE_LENGTH];

//     while (fgets(line, sizeof(line), fp)) {
//         if (line[0] != ':') {
//             continue; // Bỏ qua các dòng không bắt đầu bằng ':'
//         }

//         int length = hex_to_byte(&line[1]);
//         int record_type = hex_to_byte(&line[7]);

//         if (record_type == 0) { // Data record
//             unsigned char *new_content = realloc(hex_file->content, hex_file->size + length);
//             if (new_content == NULL) {
//                 printf("Failed to allocate memory for file: %s\n", filename);
//                 free(hex_file->content);
//                 fclose(fp);
//                 return;
//             }
//             hex_file->content = new_content;

//             for (int i = 0; i < length; i++) {
//                 unsigned char byte = hex_to_byte(&line[9 + i * 2]);
//                 hex_file->content[hex_file->size++] = byte; // Lưu byte vào mảng
//             }
//         }
//     }

//     fclose(fp);
// }

// // Hàm kiểm tra xem file có phần mở rộng .hex không
// int has_hex_extension(const char *filename) {
//     const char *dot = strrchr(filename, '.');
//     return (dot && strcmp(dot, ".hex") == 0);
// }

// // Hàm đọc các file .hex trong thư mục
// int read_hex_files(const char *dir_name, HexFile *hex_files, int *file_count) {
//     WIN32_FIND_DATA find_data;
//     HANDLE hFind;
//     char search_path[1024];

//     snprintf(search_path, sizeof(search_path), "%s\\*.hex", dir_name);
//     hFind = FindFirstFile(search_path, &find_data);

//     if (hFind == INVALID_HANDLE_VALUE) {
//         printf("Error opening directory: %s\n", dir_name);
//         return *file_count;
//     }

//     do {
//         if (has_hex_extension(find_data.cFileName)) {
//             HexFile hex_file;
//             snprintf(hex_file.name, sizeof(hex_file.name), "%s\\%s", dir_name, find_data.cFileName);
//             read_hex_file(hex_file.name, &hex_file);
//             hex_files[(*file_count)++] = hex_file; // Lưu vào mảng hex_files
//         }
//     } while (FindNextFile(hFind, &find_data) != 0);

//     FindClose(hFind);
//     return *file_count;
// }

// // Hàm ghi thông tin vào file .hala
// void write_hala_file(const char *filename, HexFile *hex_file) {
//     FILE *fp = fopen(filename, "w");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     fprintf(fp, "File: %s\nSize: %zu bytes\nContent:\n", hex_file->name, hex_file->size);
//     for (size_t j = 0; j < hex_file->size; j++) {
//         fprintf(fp, "%02X ", hex_file->content[j]);
//         if ((j + 1) % 16 == 0) {
//             fprintf(fp, "\n"); // Xuống dòng mỗi 16 byte
//         }
//     }
//     if (hex_file->size % 16 != 0) {
//         fprintf(fp, "\n"); // Xuống dòng nếu chưa đủ 16 byte
//     }
//     fprintf(fp, "\n");

//     fclose(fp);
// }

// int main() {
//     HexFile hex_files[100]; // Giả sử có thể có tối đa 100 file
//     int file_count = 0;

//     // Đọc các file .hex trong thư mục Application
//     read_hex_files(APPLICATION_FOLDER, hex_files, &file_count);

//     // Ghi thông tin vào file .hala cho mỗi file .hex
//     for (int i = 0; i < file_count; i++) {
//         // Tạo tên file .hala từ người dùng
//         char hala_file_name[HEX_FILE_NAME_LEN];
//         printf("Nhap ten file .hala cho file %s (khong bao gom phan mo rong):\n", hex_files[i].name);
//         scanf("%s", hala_file_name);
//         strcat(hala_file_name, ".hala"); // Thêm phần mở rộng .hala
//         // Ghi thông tin vào file .hala
//         write_hala_file(hala_file_name, &hex_files[i]);
        
//         // Giải phóng bộ nhớ sau khi ghi file
//         free(hex_files[i].content);
//     }

// //     // Ghi thông tin vào file .hala cho mỗi file .hex
// //     for (int i = 0; i < file_count; i++) {
// //         // Tạo tên file .hala tương ứng
// //         char hala_file_name[HEX_FILE_NAME_LEN];
// //         // Chỉ lấy phần tên file mà không có phần mở rộng
// //         char *dot = strrchr(hex_files[i].name, '.');
// //         if (dot) {
// //             *dot = '\0'; // Xóa phần mở rộng .hex
// //         }
// //         snprintf(hala_file_name, sizeof(hala_file_name), "%s.hala", hex_files[i].name);
// //         // Ghi thông tin vào file .hala
// //         write_hala_file(hala_file_name, &hex_files[i]);
        
// //         // Giải phóng bộ nhớ sau khi ghi file
// //         free(hex_files[i].content);
// //     }
//     printf("Processed %d .hex files.\n", file_count);
//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <windows.h>
// #include <string.h>

// #define MAX_LINE_LENGTH 256
// #define HEX_FILE_NAME_LEN 256
// #define APPLICATION_FOLDER "Application" // Thư mục chứa file .hex

// typedef struct {
//     char name[HEX_FILE_NAME_LEN];
//     size_t size; // Kích thước dữ liệu
//     unsigned char *content; // Dữ liệu của file .hex, không cần kích thước cố định
// } HexFile;

// // Hàm chuyển đổi 2 ký tự hexa thành một byte
// unsigned char hex_to_byte(const char *hex) {
//     unsigned char byte = 0;
//     for (int i = 0; i < 2; i++) {
//         byte <<= 4;
//         if (hex[i] >= '0' && hex[i] <= '9') {
//             byte |= hex[i] - '0';
//         } else if (hex[i] >= 'A' && hex[i] <= 'F') {
//             byte |= hex[i] - 'A' + 10;
//         } else if (hex[i] >= 'a' && hex[i] <= 'f') {
//             byte |= hex[i] - 'a' + 10;
//         }
//     }
//     return byte;
// }

// // Hàm đọc nội dung file .hex
// void read_hex_file(const char *filename, HexFile *hex_file) {
//     FILE *fp = fopen(filename, "r");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     hex_file->size = 0; // Khởi tạo kích thước
//     hex_file->content = NULL; // Khởi tạo con trỏ nội dung
//     char line[MAX_LINE_LENGTH];

//     while (fgets(line, sizeof(line), fp)) {
//         if (line[0] != ':') {
//             continue; // Bỏ qua các dòng không bắt đầu bằng ':'
//         }

//         int length = hex_to_byte(&line[1]);
//         int record_type = hex_to_byte(&line[7]);

//         if (record_type == 0) { // Data record
//             unsigned char *new_content = realloc(hex_file->content, hex_file->size + length);
//             if (new_content == NULL) {
//                 printf("Failed to allocate memory for file: %s\n", filename);
//                 free(hex_file->content);
//                 fclose(fp);
//                 return;
//             }
//             hex_file->content = new_content;

//             for (int i = 0; i < length; i++) {
//                 unsigned char byte = hex_to_byte(&line[9 + i * 2]);
//                 hex_file->content[hex_file->size++] = byte; // Lưu byte vào mảng
//             }
//         }
//     }

//     fclose(fp);
// }

// // Hàm kiểm tra xem file có phần mở rộng .hex không
// int has_hex_extension(const char *filename) {
//     const char *dot = strrchr(filename, '.');
//     return (dot && strcmp(dot, ".hex") == 0);
// }

// // Hàm đọc các file .hex trong thư mục
// int read_hex_files(const char *dir_name, HexFile *hex_files, int *file_count) {
//     WIN32_FIND_DATA find_data;
//     HANDLE hFind;
//     char search_path[1024];

//     snprintf(search_path, sizeof(search_path), "%s\\*.hex", dir_name);
//     hFind = FindFirstFile(search_path, &find_data);

//     if (hFind == INVALID_HANDLE_VALUE) {
//         printf("Error opening directory: %s\n", dir_name);
//         return *file_count;
//     }

//     do {
//         if (has_hex_extension(find_data.cFileName)) {
//             HexFile hex_file;
//             snprintf(hex_file.name, sizeof(hex_file.name), "%s\\%s", dir_name, find_data.cFileName);
//             read_hex_file(hex_file.name, &hex_file);
//             hex_files[(*file_count)++] = hex_file; // Lưu vào mảng hex_files
//         }
//     } while (FindNextFile(hFind, &find_data) != 0);

//     FindClose(hFind);
//     return *file_count;
// }

// // Hàm ghi thông tin vào file .hala
// void write_hala_file(const char *filename, HexFile *hex_file) {
//     FILE *fp = fopen(filename, "w");
//     if (!fp) {
//         printf("Cannot open file: %s\n", filename);
//         return;
//     }

//     fprintf(fp, "File: %s\nSize: %zu bytes\nContent:\n", hex_file->name, hex_file->size);
//     for (size_t j = 0; j < hex_file->size; j++) {
//         fprintf(fp, "%02X ", hex_file->content[j]);
//         if ((j + 1) % 16 == 0) {
//             fprintf(fp, "\n"); // Xuống dòng mỗi 16 byte
//         }
//     }
//     if (hex_file->size % 16 != 0) {
//         fprintf(fp, "\n"); // Xuống dòng nếu chưa đủ 16 byte
//     }
//     fprintf(fp, "\n");

//     fclose(fp);
// }

// int main() {
//     HexFile hex_files[100]; // Giả sử có thể có tối đa 100 file
//     int file_count = 0;

//     // Đọc các file .hex trong thư mục Application
//     read_hex_files(APPLICATION_FOLDER, hex_files, &file_count);

//     // Ghi thông tin vào file .hala cho mỗi file .hex
//     for (int i = 0; i < file_count; i++) {
//         // Tạo tên file .hala tương ứng
//         char hala_file_name[HEX_FILE_NAME_LEN];
//         // Chỉ lấy phần tên file mà không có phần mở rộng
//         char *dot = strrchr(hex_files[i].name, '.');
//         if (dot) {
//             *dot = '\0'; // Xóa phần mở rộng .hex
//         }
//         snprintf(hala_file_name, sizeof(hala_file_name), "%s.hala", hex_files[i].name);

//         // Ghi thông tin vào file .hala
//         write_hala_file(hala_file_name, &hex_files[i]);
        
//         // Giải phóng bộ nhớ sau khi ghi file
//         free(hex_files[i].content);
//     }

//     printf("Processed %d .hex files.\n", file_count);
//     return 0;
// }
