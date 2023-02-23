CC = g++
CFLAGS = -std=c++14 -g -Wall
DEPS = Socket.h
OBJ = socket.o 

%.o: %.cc $(DEPS)
				$(CC) -c -o $@ $< $(CFLAGS)

netcpsend: $(OBJ) netcpsend.o
				$(CC) -o $@ $^ $(CFLAGS)
NetcpReceive: $(OBJ) NetcpReceive.o
				$(CC) -o $@ $^ $(CFLAGS)
				
all: netcpsend NetcpReceive

clean:
				rm netcpsend *.o
				rm NetcpReceive
