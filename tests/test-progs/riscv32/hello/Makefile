# Makefile for building hello.elf using riscv32-unknown-elf-gcc

TARGET = hello.elf
SRC = hello.c

CC = riscv32-unknown-elf-gcc
CFLAGS = -Wall -O2

compile: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
