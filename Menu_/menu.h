#ifndef MENU_H
#define MENU_H

// Khai báo kiểu con trỏ hàm cho các chức năng menu
typedef void (*FeatureFunc)();

// Khai báo struct cho mỗi tùy chọn của menu
typedef struct {
    char *option;       // Tên tùy chọn
    char *description;  // Mô tả của tùy chọn
    FeatureFunc func;   // Hàm chức năng tương ứng
} MenuItem;

// Hàm khởi tạo và hiển thị menu
void createMenu(MenuItem *menuItems, int num_options);

#endif // MENU_H
