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
#include <stdexcept>
#include <sys/mman.h>

   

sockaddr_in make_ip_address(int port, const std::string& ip_address = std::string());

class Socket {
public:
	Socket(const sockaddr_in& address);
	~Socket();

//void send_to(const struct Message& message, const sockaddr_in& address);
//	int send_to(void *const &message, int size, const sockaddr_in& address);
	int send_to(const void * message, int size, const sockaddr_in& address);

//	int receive_from(void *const & message, int memsize, sockaddr_in& address);
	int receive_from(void * message, int memsize, sockaddr_in& address);

private:
	int fd_;
};

