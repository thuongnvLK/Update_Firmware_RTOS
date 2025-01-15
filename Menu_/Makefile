# Tên file thực thi
TARGET = Menu

# Compiler
CC = gcc

# Các cờ dùng để biên dịch
CFLAGS = -Wall -g

# Các file nguồn
SRCS = main.c menu.c

# Các file đối tượng sẽ được tạo từ file nguồn
OBJS = $(SRCS:.c=.o)

# Luật biên dịch
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Luật để biên dịch các file .o từ file .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Chạy chương trình sau khi biên dịch
run: $(TARGET)
	./$(TARGET)

# Dọn dẹp file thực thi và file đối tượng
clean:
	rm -f $(TARGET) $(OBJS)

# Tạo lại toàn bộ chương trình và chạy
rebuild: clean $(TARGET) run
