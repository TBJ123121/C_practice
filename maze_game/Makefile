#資料夾
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# 編譯器與參數
CC = gcc
TARGET = maze_game
CFLAGS = -Wall -Wextra -I$(INC_DIR)

#所有.c對應的.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# 目標
$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
# $@ → 自動變數，代表「目前這個目標 (target) 的名字」
# $^ → 自動變數，代表「這個目標的所有依賴檔案 (prerequisites)」
	$(CC) $(CFLAGS) -o $@ $^

# 編譯每個.c檔案為.o檔案
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
# $< → 自動變數，代表「第一個依賴檔案」
	$(CC) $(CFLAGS) -c $< -o $@

#建立資料夾
$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

.PHONY: all run clean

all: $(BIN_DIR)/$(TARGET)
run: all
	@echo "Running maze game..."
	$(BIN_DIR)/$(TARGET)
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/$(TARGET)
