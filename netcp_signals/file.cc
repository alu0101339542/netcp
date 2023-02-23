#include "file.h"

//Clase File
File:: File(const char* pathname, bool writeonly, off_t flength) {
  int flags;
  if(writeonly == true)
    flags = O_RDWR | O_CREAT;
  else
    flags = O_RDONLY;
  fd = open(pathname, flags,0666);
  if (fd < 0) {
    throw std::system_error(errno, std::system_category(), "no se pudo crear el archivo");
  }
  //hacer mmap
  lockf(fd, F_LOCK, 0); //bloquea el archivo para que cuando este en lectura otro no lo pueda escribir
  if (writeonly == true){
    int error = ftruncate(fd, flength);
    if (error < 0) {
      throw std::system_error(errno, std::system_category(), "Truncar fichero");
    }
    int length = lseek( fd, 0, SEEK_END ); 
    fsize=length;
    filemap = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0); //Ya tenemos el mapa creado
    if (filemap == MAP_FAILED) {
      throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }
  }
  else {
    int length = lseek( fd, 0, SEEK_END ); 
    fsize = length;
    filemap = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0); //Ya tenemos el mapa creado
    if (filemap == MAP_FAILED) {
      throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }

  }
}
  
File:: ~File(){ //Tendra que desmapear cuando acabe hay que hacer un desbloqueo
  lockf(fd, F_ULOCK, 0);
  munmap(filemap,fsize);
  close(fd);
}

int File::GetFsize(){
  return fsize;
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
  return filemap;
}

