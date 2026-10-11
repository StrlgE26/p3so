CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS =
LDLIBS = -lncurses -ltinfo -lsodium
TARGET = acceso
OBJECTS = main.o auth.o ui.o

.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)
main.o: main.c auth.h ui.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c main.c -o main.o
auth.o: auth.c auth.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c auth.c -o auth.o
ui.o: ui.c ui.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c ui.c -o ui.o
clean:
	rm -f $(OBJECTS) $(TARGET)
