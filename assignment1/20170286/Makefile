CC=gcc
CFLAGS=-g -Wall
OBJS=20170286.o
TARGET=20170286
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) 
20170286.o:20170286.c 20170286.h
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm -f *.o
	rm -f $(TARGET)
