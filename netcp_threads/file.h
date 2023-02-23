#include <cstddef>
#include <cerrno>
#include <array>
#include <string>
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
#include <atomic>
#include <sys/mman.h>

class File {
  public:
    File(const char* pathname, bool writeonly = true, off_t flength = 0);
    ~File();
    ssize_t ReadFile(void* buf, size_t count);
    ssize_t WriteFile(void* buf, size_t count); //hay que pasarle como parametro una string que va a ser donde guarde el texto leido en lugar de imprimirlo por pantalla
    void *data();
    int GetFsize();
  private:
    int fd;
    void *filemap;
    int fsize;
};

struct InitMessage {
  std::string filename;  
  off_t  mess_length; //Tamaño del mensaje para el ftruncate. el tamaño tiene que ser un off_t
};


