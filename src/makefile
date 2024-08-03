# Makefile for Text Editor Project

CC = gcc
CFLAGS = -g
OBJS = main.o terminal.o input.o output.o append_buffer.o file_io.o row_operations.o editor_operations.o
TARGET = TextEditor

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c main.h terminal.h input.h output.h append_buffer.h file_io.h row_operations.h editor_operations.h
	$(CC) $(CFLAGS) -c main.c

terminal.o: terminal.c main.h terminal.h
	$(CC) $(CFLAGS) -c terminal.c

input.o: input.c main.h input.h terminal.h editor_operations.h
	$(CC) $(CFLAGS) -c input.c

output.o: output.c main.h output.h append_buffer.h
	$(CC) $(CFLAGS) -c output.c

append_buffer.o: append_buffer.c main.h append_buffer.h
	$(CC) $(CFLAGS) -c append_buffer.c

file_io.o: file_io.c main.h file_io.h
	$(CC) $(CFLAGS) -c file_io.c

row_operations.o: row_operations.c main.h row_operations.h
	$(CC) $(CFLAGS) -c row_operations.c

editor_operations.o: editor_operations.c main.h editor_operations.h
	$(CC) $(CFLAGS) -c editor_operations.c

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET)
