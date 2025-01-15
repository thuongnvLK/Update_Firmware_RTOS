#include "uart.h"
#include <stdio.h>

// Hàm tạo độ trễ bằng vòng lặp
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 1000; i++) {
        __asm("nop");  // Lệnh NOP để tạo độ trễ
    }
}

int main(void) {
    uart_init();  // Khởi tạo UART

   // (void)printf("UART\r\n");  // Test printf qua UART

    while (1) {
        uart_send_string("Motor is running ...... !!! \r\n");
        delay_ms(1000);  // Tạo độ trễ 1 giây
    }
}