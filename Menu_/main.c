#include <stdio.h>
#include "menu.h"

// Các hàm chức năng cho từng dòng menu
void feature1() {
    printf("Feature 1: Hello, World!\n");
}

void feature2() {
    printf("Feature 2: This is an example.\n");
}

void feature3() {
    printf("Feature 3: Use arrow keys to move.\n");
}

void feature4() {
    printf("Feature 4: Fourth line.\n");
}

void feature5() {
    printf("Feature 5: Fifth line.\n");
}

int main() {
    // Mảng các tùy chọn, mô tả và chức năng được cấu hình qua struct MenuItem
    MenuItem menuItems[] = {
        {"Option 1", "Hello, World!", feature1},
        {"Option 2", "This is an example", feature2},
        {"Option 3", "Use arrow keys to move", feature3},
        {"Option 4", "Fourth line", feature4},
        {"Option 5", "Fifth line", feature5}
    };

    // Số lượng tùy chọn
    int num_options = sizeof(menuItems) / sizeof(menuItems[0]);

    // Tạo và hiển thị menu
    createMenu(menuItems, num_options);

    return 0;
}
