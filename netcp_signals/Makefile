CC = g++
CFLAGS = -std=c++14 -g -Wall -pthread
DEPS = Socket.h file.h
OBJ = socket.o file.o

%.o: %.cc $(DEPS)
				$(CC) -c -o $@ $< $(CFLAGS)

main_netcp: $(OBJ) main_netcp.o 
				$(CC) -o $@ $^ $(CFLAGS)
			
all: main_netcp

clean:
				rm -rf kk* main_netcp *.o
