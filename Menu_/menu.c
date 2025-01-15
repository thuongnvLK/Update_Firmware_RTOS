#include <stdio.h>
#include <windows.h>  // Thư viện cho hàm API của Windows
#include "menu.h"

/**
 * @brief Thiết lập vị trí con trỏ trong console Windows.
 * 
 * @param x Tọa độ x của con trỏ (cột).
 * @param y Tọa độ y của con trỏ (hàng).
 * 
 * @return void
 * 
 * @note Sử dụng API của Windows: SetConsoleCursorPosition.
 *       Không kiểm tra lỗi trả về từ API.
 */
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/**
 * @brief Xóa màn hình console Windows.
 * 
 * @return void
 * 
 * @note Sử dụng lệnh hệ thống "cls" để xóa màn hình.
 */
void clearScreen() {
    system("cls"); // Lệnh hệ thống để xóa màn hình
}

/**
 * @brief Lấy trạng thái của các phím nhấn (bao gồm Enter, ESC, mũi tên lên/xuống).
 * 
 * @return int Mã phím đã được nhấn. Nếu không có phím nào được nhấn, trả về 0.
 * 
 * @note Sử dụng API của Windows: GetAsyncKeyState để lấy trạng thái của phím.
 */
int getKeyPress() {
    if (GetAsyncKeyState(VK_UP)) {
        return VK_UP;
    }
    if (GetAsyncKeyState(VK_DOWN)) {
        return VK_DOWN;
    }
    if (GetAsyncKeyState(VK_RETURN)) {
        return VK_RETURN;
    }
    if (GetAsyncKeyState(VK_ESCAPE)) {
        return VK_ESCAPE;
    }
    return 0; // Không có phím được nhấn
}

/**
 * @brief Tạo và quản lý menu trong console Windows.
 * 
 * @param menuItems Mảng chứa các mục menu.
 * @param num_options Số lượng tùy chọn trong menu.
 * 
 * @return void
 * 
 * @note Hàm này thực hiện vẽ menu, xử lý sự kiện phím nhấn để di chuyển giữa các tùy chọn menu.
 */
void createMenu(MenuItem *menuItems, int num_options) {
    int current_row = 0;
    static int check = 1;
    // Vòng lặp chính để quản lý menu
    while (1) {
        clearScreen(); // Xóa màn hình mỗi khi cần vẽ lại

        // In ra các tùy chọn menu
        for (int i = 0; i < num_options; i++) {
            if (i == current_row) {
                printf("> %s: %s\n", menuItems[i].option, menuItems[i].description);  // Dòng được chọn
            } else {
                printf("  %s: %s\n", menuItems[i].option, menuItems[i].description);
            }
        }

        // Đợi và kiểm tra phím bấm
        int key = 0;
        while ((key = getKeyPress()) == 0) {
            Sleep(50);  // Đợi trong 50ms giữa các lần kiểm tra
        }
        
        // Xử lý lần nhấn Enter đầu tiên
        if (check == 1 && key == VK_RETURN) {
            key = 0;
            check = 0;
        }
        
        // Xử lý khi nhấn phím mũi tên lên
        if (key == VK_UP) {
            if (current_row > 0) {
                current_row--;
            }
            while (GetAsyncKeyState(VK_UP) & 0x8000) {
                Sleep(100);  // Chờ cho phím nhả ra
            }
        // Xử lý khi nhấn phím mũi tên xuống
        } else if (key == VK_DOWN) {
            if (current_row < num_options - 1) {
                current_row++;
            }
            while (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                Sleep(100);  // Chờ cho phím nhả ra
            }
        // Xử lý khi nhấn Enter để chọn mục menu
        } else if (key == VK_RETURN) {
            clearScreen(); // Xóa màn hình trước khi thực hiện chức năng
            menuItems[current_row].func(); // Gọi hàm tương ứng với tùy chọn
            printf("Press ESC key to return...\n");
            while (getKeyPress() != VK_ESCAPE) {
                Sleep(100);  // Đợi người dùng nhấn ESC để quay lại menu
            }
        }
    }
}
