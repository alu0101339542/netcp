#include "Socket.h"

sockaddr_in make_ip_address(int port, const std::string& ip_address){
  // Dirección del socket local
  struct  in_addr addr; 
  sockaddr_in local_address{};	// Así se inicializa a 0, como se recomienda
  local_address.sin_family = AF_INET;// Pues el socket es de dominio AF_INET	
  
  if(ip_address.length() == 0)
    local_address.sin_addr.s_addr = INADDR_ANY;
  else{
    if(inet_aton(ip_address.c_str(), &addr) == 0){
      std::cerr << "falló addr: " << std::strerror(errno) << '\n';
      //return -1 ;
    }
    local_address.sin_addr.s_addr = *(in_addr_t *)&addr;
    //local_address.sin_addr.s_addr = reinterpret_cast<in_addr_t>(&addr);
  }
  local_address.sin_port = htons(port);
  return local_address;
}

Socket::Socket(const sockaddr_in& address) {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0); //creates an endpoint for communication and returns a descriptor
  if (fd_ < 0) {
    throw std::system_error(errno, std::system_category(), "no se pudo crear el socket");
  }
  int result = bind(fd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)); //Devuelve un entero y le asigna al socket una  direccion	
  if (result < 0) {
    throw std::system_error(errno, std::system_category(), "error al crear el bind");

  }
}
		  
Socket::~Socket() {
  close(fd_);
}

void Socket::send_to(const Message& message, const sockaddr_in& address) {
  int result = sendto(fd_, &message, sizeof(message), 0,
	                    reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr));
  if (result < 0) {
    std::cerr << "falló sendto: " << std::strerror(errno) << '\n';
    //return 6;	// Error. Terminar con un valor diferente y > 0
  }
}

void Socket::receive_from(Message& message, sockaddr_in& address) {
  socklen_t src_len = sizeof(address);
  int result = recvfrom(fd_, &message, sizeof(message), 0,
	                      reinterpret_cast<sockaddr*>(&address), &src_len);
  if (result < 0) {
    std::cerr << "falló recvfrom: " << std::strerror(errno) << '\n';
    //return 8;
  }
}

//Clase File
File:: File(const char* pathname, int flags, mode_t mode) {
  fd = open( pathname,flags, mode);
  if (fd < 0)
    throw std::system_error(errno, std::system_category(), "no se pudo crear el archivo");
}

File:: ~File(){
  close(fd);
}

ssize_t File::ReadFile(void* buf, size_t count) {
  ssize_t readnum = read(fd, buf, count);
  if (readnum < 0) {
    std::cerr << "falló readnum: " << std::strerror(errno) << '\n';
    return 8;
  }
  return readnum;
}

ssize_t File::WriteFile(void* buf, size_t count) {
  ssize_t writenum = write(fd, buf, count);
  if (writenum < 0) {
    std::cerr << "falló writenum: " << std::strerror(errno) << '\n';
    return 9;
     
  }
  return writenum;
}