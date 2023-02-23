#include "Socket.h"
#include "file.h"

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

int Socket::send_to(const void *message, int size, const sockaddr_in& address) {  //Tenemos que hacer que actue directamente sobre la memoria mapeada para ello cogemos el puntero y el tamaño del map
//Se harian multiples llamadas a send to pero no al read. Haciendolo de esta manera habria que incrementar el puntero en 60k
  int result = sendto(fd_, message, size, 0, //porque &message se interpreta como buf cuando contiene mas cosas
	                    reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr));
  if (result < 0) {
    std::cerr << "falló sendto: " << std::strerror(errno) << '\n';
    //return 6;	// Error. Terminar con un valor diferente y > 0
  }
  return result;
}
/*
void Socket::send_to(const Message& message, const sockaddr_in& address) {  //Tenemos que hacer que actue directamente sobre la memoria mapeada para ello cogemos el puntero y el tamaño del map
//Se harian multiples llamadas a send to pero no al read. Haciendolo de esta manera habria que incrementar el puntero en 60k
  int result = sendto(fd_, &message, sizeof(message), 0, //porque &message se interpreta como buf cuando contiene mas cosas
	                    reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr));
  if (result < 0) {
    std::cerr << "falló sendto: " << std::strerror(errno) << '\n';
    //return 6;	// Error. Terminar con un valor diferente y > 0
  }
}
*/

int Socket::receive_from(void * message, int memsize, sockaddr_in& address) { //Tenemos que hacer que actue directamente sobre la memoria mapeada. Actundo de forma parecida al send to
//Hay que crear un fichero de recepcion con un nombre temporal ya que mas adelante en la practica lo cambiaremos. Esto lo hacemos con el open(). O podemos hacer tambien que el sendto
//envie una estructura de mensaje que contenga el nombre del fichero

//Una vez recibido el filename en el mensaje se lo pasamos a open como primer parametro, pero claro al crear el fichero por primera vez tiene tamaño 0. Lo cambiamos 
//con ftruncate() recibe como primer parametro el descriptor del archivo que devuelve open() y como segndo el tamaño que tiene que ser el mismo que el que mandamos en el send to
// por tanto lo logico seria incluir en la estructura del mensaje tambien el tamaño del mismo 

//Una vez hecho esto podemos usar el mmap para reservar en el espacio de direcciones del proceso un hueco de direcciones en el que ya el fichero con su tamaño va a ser accesible. Este se abriria para modo escritura y el de send para lectura
  socklen_t src_len = sizeof(address);
  int result = recvfrom(fd_, message, memsize, 0,
	                      reinterpret_cast<sockaddr*>(&address), &src_len);
  if (result < 0) {
    std::cerr << "falló recvfrom: " << std::strerror(errno) << '\n';
    return -1;
  }
  return result;
}



