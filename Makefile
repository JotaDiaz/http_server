CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = servidor
SRCS = server.c http_handler.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
run: $(TARGET)
	./$(TARGET)
