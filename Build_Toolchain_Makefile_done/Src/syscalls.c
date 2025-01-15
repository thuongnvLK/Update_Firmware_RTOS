#include "uart.h"
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h> // required for _write()

// Hàm này sẽ được gọi khi sử dụng printf
int _write(int file, char *ptr, int len) {
    // Kiểm tra trường hợp đầu ra (stdout, stderr)
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (int i = 0; i < len; i++) {
            uart_send_char(ptr[i]);  // Gửi từng ký tự qua UART
        }
        return len;  // Trả về số byte đã ghi
    }
    return -1;  // Nếu không phải stdout hoặc stderr, trả về lỗi
}
