CC:=gcc
CFLAGS:=-Wall -g
EXE:=student_info

all: $(EXE)

$(EXE): student_info.c
	@echo "Building student information management system..."
	$(CC) student_info.c $(CFLAGS) -o $(EXE)

# Clean up the compiled files
clean:
	@echo "Cleaning up compiled files..."
	rm -f $(EXE)

