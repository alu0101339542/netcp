#include "Socket.h"
class File {
public:
  File(const char* pathname, bool writeonly = true, off_t flength = 0);
  ~File();
  ssize_t ReadFile(void* buf, size_t count);
  ssize_t WriteFile(void* buf, size_t count); //hay que pasarle como parametro una string que va a ser donde guarde el texto leido en lugar de imprimirlo por pantalla
  void *data();
private:
  int fd;
  void *filemap:
};

//Esto tiene que ser otro mensaje el que le pasemos primero
// Estructura de los mensajes
struct Message {
  size_t size = 1024;
  std::array<char, 1024> text;	// Igual que "char text[1024]"
};

struct InitMessage {
  std::string filename;  
  off_t  mess_length; //Tamaño del mensaje para el ftruncate. el tamaño tiene que ser un off_t
}
