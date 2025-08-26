CC = gcc
CFLAGS = -Wall -Wextra -g
SRCS = src/main.c src/namespace.c src/filesystem.c src/cgroups.c src/container.c src/network.c
OBJ = $(SRCS:.c=.o)
TARGET = mini_container
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)