#include "Socket.h"
int protected_main(void) {
  // Dirección del socket remoto
  sockaddr_in local_address{};	// Porque se recomienda inicializar a 0
  sockaddr_in remote_address{};
  local_address = make_ip_address(3000, "127.0.0.1");
  Socket netreceive(local_address);
  Message message;
  while(1) {
    // Vamos a mostrar el mensaje recibido en la terminal
    netreceive.receive_from(message, remote_address);
    // Primero convertimos la dirección IP como entero de 32 bits
    // en una cadena de texto.
    char* remote_ip = inet_ntoa(remote_address.sin_addr);

    // Recuperamos el puerto del remitente en el orden adecuado para nuestra CPU
    int remote_port = ntohs(remote_address.sin_port);

    // Imprimimos el mensaje y la dirección del remitente
    //message.text[1023] = '\0';
    std::string text(message.text.data(), message.text.size());//Escribe size bytes
    std::cout << "El sistema " << remote_ip << ":" << remote_port <<
                 " envió el mensaje '" << text << "'\n";
  }
}
int main(void) {
  try {
    return protected_main();
  }	
  catch(std::bad_alloc& e) {
    std::cerr << "mytalk" << ": memoria insuficiente\n";
    return 1;
  }
  catch(std::system_error& e) {
    std::cerr << "mitalk" << ": " << e.what() << '\n';
    return 2;
  }
  catch (...) {
    std::cout << "Error desconocido\n";
    return 99;
  }
}