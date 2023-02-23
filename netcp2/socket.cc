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

void Socket::send_to(const Message& message, const sockaddr_in& address) {  //Tenemos que hacer que actue directamente sobre la memoria mapeada para ello cogemos el puntero y el tamaño del map
//Se harian multiples llamadas a send to pero no al read. Haciendolo de esta manera habria que incrementar el puntero en 60k
  int result = sendto(fd_, &message, sizeof(message), 0, //porque &message se interpreta como buf cuando contiene mas cosas
	                    reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr));
  if (result < 0) {
    std::cerr << "falló sendto: " << std::strerror(errno) << '\n';
    //return 6;	// Error. Terminar con un valor diferente y > 0
  }
}

void Socket::receive_from(Message& message, sockaddr_in& address) { //Tenemos que hacer que actue directamente sobre la memoria mapeada. Actundo de forma parecida al send to
//Hay que crear un fichero de recepcion con un nombre temporal ya que mas adelante en la practica lo cambiaremos. Esto lo hacemos con el open(). O podemos hacer tambien que el sendto
//envie una estructura de mensaje que contenga el nombre del fichero

//Una vez recibido el filename en el mensaje se lo pasamos a open como primer parametro, pero claro al crear el fichero por primera vez tiene tamaño 0. Lo cambiamos 
//con ftruncate() recibe como primer parametro el descriptor del archivo que devuelve open() y como segndo el tamaño que tiene que ser el mismo que el que mandamos en el send to
// por tanto lo logico seria incluir en la estructura del mensaje tambien el tamaño del mismo 

//Una vez hecho esto podemos usar el mmap para reservar en el espacio de direcciones del proceso un hueco de direcciones en el que ya el fichero con su tamaño va a ser accesible. Este se abriria para modo escritura y el de send para lectura
  socklen_t src_len = sizeof(address);
  int result = recvfrom(fd_, &message, sizeof(message), 0,
	                      reinterpret_cast<sockaddr*>(&address), &src_len);
  if (result < 0) {
    std::cerr << "falló recvfrom: " << std::strerror(errno) << '\n';
    //return 8;
  }
}

//Clase File
File:: File(const char* pathname, bool writeonly = true, off_t flength = 0) {
  int flags;
  if(writeonly == true)
    flags = O_WRONLY;
  else
    flags = O_RDONLY;
  fd = open(pathname, flags);
  if (fd < 0) {
    throw std::system_error(errno, std::system_category(), "no se pudo crear el archivo");
  }
  //hacer nmap
  int erlockf = lockf(fd, F_LOCK, 0);
  if (writeonly == true){
    int error = ftruncate(fd, flength);
    int length = lseek( fd, 0, SEEK_END ); 
    filemap = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0); //Ya tenemos el mapa creado
    if (filemap == MAP_FAILED) {
      throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }
  }
  else {
    int length = lseek( fd, 0, SEEK_END ); 
    filemap = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0); //Ya tenemos el mapa creado
    if (filemap == MAP_FAILED) {
      throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }

  }
}
  
File:: ~File(){ //Tendra que desmapear cuando acabe hay que hacer un desbloqueo
  lockf(fd, F_ULOCK, 0);
  munmap(filemap);
  close(fd);
}

ssize_t File::ReadFile(void* buf, size_t count) {
  ssize_t readnum = read(fd, buf, count);//reemplazamos la llamada al sistema por el map
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

void* File::data(void){
  return *filemap;
}

int Quest3(void) {
  //Creamos el directorio
  int mkdirerror;
  //guardar argv2
  mkdirerror = mkdir(argv[2]; 0777);
  // Dirección del socket remoto
  sockaddr_in local_address{};	// Porque se recomienda inicializar a 0
  sockaddr_in remote_address{};
  local_address = make_ip_address(3000, "127.0.0.1");
  Socket netreceive(local_address);
  Message message;
  InitMessage initmessage;
  netreceive.receive_from(initmessage, remote_address);
  File recevedfile();
  //antes de recibir el mensaje tendria que recibir el otro que contiene el filename para luego hacer el open y guardarle la informacion en el receive
  while(1) {
    // Vamos a mostrar el mensaje recibido en la terminal
    netreceive.receive_from(message, remote_address);
    // Primero convertimos la dirección IP como entero de 32 bits
    // en una cadena de texto.
    char* remote_ip = inet_ntoa(remote_address.sin_addr);

    // Recuperamos el puerto del remitente en el orden adecuado para nuestra CPU
    int remote_port = ntohs(remote_address.sin_port);

    // Imprimimos el mensaje y la dirección del remitente
    std::string text(message.text.data(), message.text.size());//Escribe size bytes

    
    //En nuestro programa tendremos que hacer el uso de write, creando un objeto file con permiso solo de escritura
    std::cout << "El sistema " << remote_ip << ":" << remote_port <<
                 " envió el mensaje '" << text << "'\n";
  }
}