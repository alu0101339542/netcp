#include "Socket.h"

#include <array>


int main(void) {
  // Dirección del socket remoto
  sockaddr_in local_address{};
  local_address = make_ip_address(3000, "127.0.0.1");
  Socket netsend(local_address);

  sockaddr_in remote_address{};	// Porque se recomienda inicializar a 0
  remote_address= make_ip_address(3000, "127.0.0.1");


  

  // Enviar un mensaje "¡Hola, mundo!" al socket remoto
  std::string message_text("¡Hola, mundo!");

  Message message;
  message_text.copy(message.text.data(), message.text.size() - 1, 0);

              // Inicializar otros campos de Message
  
  netsend.send_to(Message, remote_address);
}
