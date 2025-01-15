// typedef enum {
//     OTA_COMMAND,
//     OTA_HEADER,
//     OTA_DATA,
//     OTA_RESPONSE
// } CommandId;

// typedef enum {
//     START_CODE,
//     STOP_CODE,
//     REQUEST_CODE
// } OTA_Code_Name;

// typedef enum {
//     ACK,
//     NACK
// } OTA_Response_Name;

// typedef struct {
//     uint8_t command_id;
//     uint8_t len;
//     uint8_t ota_code;
// } __attribute__((packed)) OTACode;

// typedef struct {
//     uint8_t command_id;
//     uint8_t len;
//     uint8_t ack;
// } __attribute__((packed)) OTAResponse;

// typedef struct {
//     uint8_t command_id;
//     uint8_t len;
//     uint8_t name[50];   
//     uint8_t version[50]; 
// } __attribute__((packed)) OTAInf;

// typedef struct {
//     uint8_t command_id;
//     uint8_t len;
//     uint8_t name[50];   
//     uint8_t version[50]; 
// } __attribute__((packed)) OTAInf;
#include <stdio.h>

unsigned char calculate_checksum(const unsigned char *data, size_t length) {
    unsigned long sum = 0; // Sử dụng long để tránh tràn số trong quá trình cộng

    // Cộng 8 byte đầu tiên
    for (size_t i = 0; i < length && i < 8; i++) {
        sum += data[i];
    }

    // Thực hiện AND với 0xFF để chỉ lấy 8 bit thấp nhất
    return (unsigned char)(sum & 0xFF);
}

int main() {
    // Dữ liệu hex được chuyển đổi thành mảng unsigned char
    unsigned char data[] = {0x50, 0x2A, 0x00, 0x20, 0x45, 0x82, 0x00, 0x08,
                             0x4D, 0x82, 0x00, 0x08, 0x4F, 0x82, 0x00, 0x08};

    size_t length = sizeof(data) / sizeof(data[0]);

    // Tính toán checksum
    unsigned char checksum = calculate_checksum(data, length);

    // In ra checksum
    printf("Checksum: 0x%02X\n", checksum);

    return 0;
}
