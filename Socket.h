#include <sys/socket.h>
#include <netinet/ip.h> //Para las estructuras de sockaddr_in
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <array>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>


sockaddr_in make_ip_address(int port, const std::string& ip_address = std::string());

class Socket {
public:
	Socket(const sockaddr_in& address);
	~Socket();

	void send_to(const struct Message& message, const sockaddr_in& address);
	void receive_from(struct Message& message, sockaddr_in& address);

private:
	int fd_;
};

class File {
public:
  File(const char* pathname, int flags, mode_t mode = 0664);
  ~File();
  ssize_t ReadFile(void* buf, size_t count);
  ssize_t WriteFile(void* buf, size_t count);
private:
  int fd;
};

// Estructura de los mensajes
  struct Message {

                // Otros campos del mensaje

    std::array<char, 1024> text;	// Igual que "char text[1024]"
                // pero recomendado así en C++

                // Otros campos del mensaje

  };