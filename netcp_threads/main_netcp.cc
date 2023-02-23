#include "Socket.h"
#include "file.h"
#include <exception>
#include <thread>
#define BLKSIZE 60000

std::atomic_bool quit_app;

int send_file(std::string fname,std::atomic_bool &quit_send){
  char *fptr;
  bool writeonly = false;
  int fsize, bleft, sent;
  sockaddr_in local_address{};
  local_address = make_ip_address(0);
  Socket netsend(local_address);
  sockaddr_in remote_address{};	// Porque se recomienda inicializar a 0
  remote_address= make_ip_address(3000, "127.0.0.1");
  File archivo(fname.c_str(), writeonly); //Tendremos que poner el permiso a solo lectura
  fptr = (char *) archivo.data();
  fsize = archivo.GetFsize();
  InitMessage protini;
  protini.filename = fname;
  protini.mess_length = fsize;
  /*FileInfo srcfile;
  srcfile.fnamesize = fname.size();
  srcfile.mess_length = fsize;
  const char * nombre = fname.c_str();
  */
  if(quit_send == true){
    std::cout << "aborting sending file" << std::endl;
    return -1;
  }
  /*sent = netsend.send_to((void *) &fsize, sizeof(int), remote_address);
  if(sent != sizeof(int)){
    std::cout << "Error sending initial protocol" << std::endl;
    return -1;
  }
  if(quit_send == true){
    std::cout << "aborting sending file" << std::endl;
    return -1;
  }
  sent = netsend.send_to((void *)fname.c_str(), fname.size(), remote_address);
  if(sent != srcfile.fnamesize){
    std::cout << "Error sending initial protocol" << std::endl;
    return -1;
  }
  */
  
  sent = netsend.send_to((void *) &protini, sizeof(protini), remote_address);
  if(sent != sizeof(protini)){
    std::cout << "Error sending initial protocol" << std::endl;
    return -1;
  }
  
 // std::cout << "protocol sent" << std::endl;
  bleft = fsize;
  int tosend;
  while (bleft > 0){
    if (bleft > BLKSIZE)
      tosend = BLKSIZE;
    else
      tosend = bleft;
    if(quit_send == true){
      std::cout << "aborting sending file" << std::endl;
      return -1;
    }
    sent = netsend.send_to((void *) fptr, tosend, remote_address);
    if (sent != tosend){
      std::cout << "Error sending packet" << std::endl;
    }
//      std::cout << "sent bytes" << sent << std::endl;

    bleft-=sent;
    fptr+=sent;
  }
  return 0;
}

//************************************************//

int receive_file(std::string outdir,std::exception_ptr& eptr,std::atomic_bool &quit_recv) {
  // Dirección del socket remoto
  bool flag=true;
  try {
    bool writeonly = true;
    char *fptr;
    sockaddr_in local_address{};	// Porque se recomienda inicializar a 0
    sockaddr_in remote_address{};

    local_address = make_ip_address(3000, "127.0.0.1");

    Socket netreceive(local_address);

    InitMessage stprot;
    int received;
    //char name[100];
    //FileInfo srcfile;
    int fsize;
    while(flag==true){
      if(quit_recv == true){
        std::cout << "aborting receiving file" << std::endl;
        flag = false;
        break;
        //return -1;
      }

      netreceive.receive_from((void *) &stprot, sizeof(stprot), remote_address);
      if(quit_recv == true){
        std::cout << "aborting receiving file" << std::endl;
        flag = false;
        break;
        //return -1;
      }
      std::string myfile = outdir + "/" + stprot.filename; 
      std::cout << "fichero a escribir: " << stprot.filename << std::endl;
      fsize=stprot.mess_length;

      File dest(myfile.c_str(), writeonly, fsize);
      fptr = (char *) dest.data();
      int bleft = fsize;
      int recsize;
      
      while(bleft > 0) { 
        if (bleft > BLKSIZE)
          recsize = BLKSIZE;
        else
          recsize = bleft;
        // Vamos a mostrar el mensaje recibido en la terminal
        if(quit_recv == true){
          std::cout << "aborting receiving file" << std::endl;
          flag = false;
          break;
          //return -1;
        }
        received = netreceive.receive_from((void *) fptr, recsize, remote_address);
        if (received == -1){
          std::cout << "Error receiving packet" << std::endl;
          return -1;
        }
        if(quit_recv == true){
          std::cout << "aborting receiving file" << std::endl;
          flag = false;
          break;
          //return -1;
        }
        // Primero convertimos la dirección IP como entero de 32 bits
        // en una cadena de texto.
        //char* remote_ip = inet_ntoa(remote_address.sin_addr);

        // Recuperamos el puerto del remitente en el orden adecuado para nuestra CPU
        //int remote_port = ntohs(remote_address.sin_port);
        bleft-=received;
        fptr+=received;

        // Imprimimos el mensaje y la dirección del remitente
      // std::string text(message.text.data(), message.text.size());//Escribe size bytes

        
        //En nuestro programa tendremos que hacer el uso de write, creando un objeto file con permiso solo de escritura
       // std::cout << "El sistema " << remote_ip << ":" << remote_port << " envió " << received << "bytes" << std::endl;
        //             " envió el mensaje '" << text << "'\n";
      }
    }
  }
  catch (...) {
		eptr = std::current_exception();
  }
  if (eptr) {
		std::rethrow_exception(eptr);
	}
  if(flag==false)
    std::cout << "Saliendo por aborto" << std::endl;
  return 0;
}

void usr1_signal_handler(int signum)
{
    quit_app = true;
}

void setup_signals()
{
    struct sigaction usr1_action = {0};
    usr1_action.sa_handler = &usr1_signal_handler;

    sigaction( SIGUSR1, &usr1_action, NULL );
}


void task_menu(std::exception_ptr& eptr){
	try {
    char option;
    std::string outdir, srcfile;
    std::thread recth, senth;
    std::atomic<bool> quit_send{false};
    std::atomic<bool> quit_recv{false};

    setup_signals();

    while(true){
      std::cout << std::string(50, '\n');
      std::cout << "Please choose an option:" << std::endl;
      std::cout << "R --> Receive a file" << std::endl;
      std::cout << "S --> Send a file" << std::endl;
      std::cout << "O --> Pause an outgoing file" << std::endl;
      std::cout << "P --> Stop receiving a file" << std::endl;
      std::cout << "Q --> Quit" << std::endl;

      std::cin >> option;
      if (option == 'R'){
        std::cout << "Please write the directory path where the file will be stored" << std::endl;
        std::cin >> outdir;
        int error = mkdir(outdir.c_str(), 0777);
        if(error == -1){
          std::cout << "Directory exists" << std::endl;
        }
        recth=std::thread(receive_file, std::ref(outdir),std::ref(eptr),std::ref(quit_recv));
        //recth.detach();
        //std::thread receive_thread(receive_file, std::ref(outdir),std::ref(eptr),std::ref(quit_recv));
        //receive_thread.detach();
      }
      else if (option == 'S'){
        std::cout << "File name to be sent:" << std::endl;
        std::cin >> srcfile;
        senth = std::thread(send_file, std::ref(srcfile),std::ref(quit_send));
        senth.join();
        //std::thread send_thread(send_file, std::ref(srcfile),std::ref(quit_send));
        //send_thread.detach();
      }
      else if (option == 'O'){
        std::cout << "Pause outgoing file" << std::endl;
        quit_send = true;
      }
      else if (option == 'P'){
        std::cout << "stop receiving a file" << std::endl;
        quit_recv = true;
        pthread_kill(recth.native_handle(), SIGUSR1);
        if (quit_app == true){
          std::cout << "Quiting by signals" << std::endl;
          return;
        }
      }
      else if (option == 'Q'){
        std::cout << "Quitting..." << std::endl;
        quit_recv = true;
        pthread_kill(recth.native_handle(), SIGUSR1);
        recth.join();
       // senth.join();
        return;
      }
      else{
        std::cout << "Option not implemented. Please select another one" << std::endl;
      }
    }
  }
  catch (...) {
		eptr = std::current_exception();
	}
  if (eptr) {
		std::rethrow_exception(eptr);
	}
}

	// En caso de excepción, el hilo saldrá del catch y terminará solo
// al salir de la función por aquí.

int protected_main(int argc, char* argv[]){
	std::exception_ptr eptr {};
  std::thread my_thread(&task_menu, std::ref(eptr));

	// Esperar a que el hilo termine... 
	my_thread.join();
  std::cout << "Fin del hilo" << std::endl;

// Si el hilo terminó con una excepción, relanzarla aquí.
  if (eptr) {
		std::rethrow_exception(eptr);
	}
  return 0;
}

int main(int argc, char* argv[])
{
	try {
    return protected_main(argc, argv);
	}
  catch(std::bad_alloc& e) {
    std::cerr << "mytalk" << ": memoria insuficiente\n";
    return 1;
  }
  catch(std::system_error& e) {
    std::cerr << "mytalk" << ": " << e.what() << '\n';
    return 2;
  }
  catch (...) {
    std::cout << "Error desconocido\n";
    return 99;
  }
}


/*void Quest1(std::string str) {
  switch(str) {
    case Receive :
      Quest3();
      break;
    case Send :

      break;
    case Abort:

      break;
    case Pause:

      break;
    case Resume:

      break;
    case Quit:

      break;
  }
}
*/