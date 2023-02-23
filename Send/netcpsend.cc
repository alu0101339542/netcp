#include "Socket.h"

int protected_main(void) {
  // Dirección del socket local
  sockaddr_in local_address{};
  local_address = make_ip_address(0);
  Socket netsend(local_address);
  sockaddr_in remote_address{};	// Porque se recomienda inicializar a 0
  remote_address= make_ip_address(3000, "127.0.0.1");
  File archivo("./prueba.txt", 0);
  // Enviar un mensaje "¡Hola, mundo!" al socket remoto
  //std::string message_text("¡Hola, mundo!");
  Message message;
  //message_text.copy(message.text.data(), message.text.size() - 1, 0);
  while(ssize_t bytes_read = archivo.ReadFile(reinterpret_cast<void *>(&message.text), message.text.size()) > 0) {
    message.size = bytes_read;
    netsend.send_to(message, remote_address);
  }
  return 0;
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
