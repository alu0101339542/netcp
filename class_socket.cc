#include "class_socket.h"

sockaddr_in make_ip_address(int port, const std::string& ip_address =
std::string()){

}

Socket::Socket(const sockaddr_in& address) {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0); //creates an endpoint for communication and returns a descriptor
  if (fd < 0) {
    std::cerr << "no se pudo crear el socket: " <<	
    std::strerror(errno) << '\n';
    return 3;	// Error. Terminar con un valor diferente y > 0
  }
  int result = bind(fd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)); //Devuelve un entero y le asigna al socket una  direccion	
  if (result < 0) {
    std::cerr << "falló bind: " << z << '\n';
    return 5;	// Error. Terminar con un valor diferente y > 0
  }
}
		  
}

Socket::~Socket() {
  close(fd_);
}

Socket:: void send_to(const Message& message, const sockaddr_in& address) {
  int result = sendto(fd_, &message, sizeof(message), 0,
	                    reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr));
  if (result < 0) {
    std::cerr << "falló sendto: " << std::strerror(errno) << '\n';
    return 6;	// Error. Terminar con un valor diferente y > 0
  }
}

Socket:: void receive_from(Message& message, sockaddr_in& address) {
  socklen_t src_len = sizeof(remote_address);
  int result = recvfrom(fd_, &message, sizeof(message), 0,
	                      reinterpret_cast<sockaddr*>(&address), &src_len);
  if (result < 0) {
    std::cerr << "falló recvfrom: " << std::strerror(errno) << '\n';
    return 8;
  }
}