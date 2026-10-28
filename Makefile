CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
TARGET = employee_dashboard
SRCS = src/main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
