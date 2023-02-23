#include "Socket.h"
#include "file.h"
#include <exception>
#include <thread>
#define BLKSIZE 60000
//Definimos tres variables atomicas para finalizar el programa
std::atomic_bool quit_app;
std::atomic_bool quit_recv;
std::atomic_bool quit_send;

void signal_handler(int signum)
{
    quit_app = true;
}

//signals_thread(&task_signal, std::ref(eptr),std::ref(my_thread));

//Thread que gestiona el uso de señales
int task_signal(std::exception_ptr& eptr,std::thread &my_thread){
  sigset_t sigwaitset; //A set of signals to be blocked, unblocked, or waited for.
  int signum;
  struct sigaction usr1_action = {0}; //Structure describing the action to be taken when a signal arrives.

  usr1_action.sa_handler = &signal_handler; //specifies the action to be associated with signum 
  sigaction( SIGHUP, &usr1_action, NULL ); //Cuando cerramos un terminal
  sigaction( SIGTERM, &usr1_action, NULL ); //mata el proceso permitiéndole terminar correctamente
  sigaction( SIGINT, &usr1_action, NULL ); //interrrupcion por teclado(ctr+c)

  sigemptyset( &sigwaitset ); //inicia  el conjunto de señales dado por conjunto al conjunto vacío,
                             // con todas las señales fuera del conjunto.
  sigaddset ( &sigwaitset, SIGHUP); //adds the individual signal specified by the signo to the signal set pointed to by set.
  sigaddset ( &sigwaitset, SIGTERM);
  sigaddset ( &sigwaitset, SIGINT);
  sigprocmask( SIG_UNBLOCK, &sigwaitset, NULL ); //cambia las señales bloqueadas

  sigwait( &sigwaitset, &signum ); //suspends execution of the calling thread until one
                                   //of the signals specified in the signal set set becomes pending.
  std::cout << "received signal " << signum << std::endl; 
  std::cout <<"sending signal" << std::endl;
  pthread_kill(my_thread.native_handle(), SIGUSR2); //Envia a la thread la señal usr2
  return 0;

}

int send_file(std::string fname){
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
  FileInfo srcfile;
  srcfile.fnamesize = fname.size();
  srcfile.mess_length = fsize;
  
  
  if(quit_send == true){
    std::cout << "aborting sending file" << std::endl;
    return -1;
  }
  //Envio el tamaño del fichero
  sent = netsend.send_to((void *) &fsize, sizeof(int), remote_address);
  if(sent != sizeof(int)){
    std::cout << "Error sending initial protocol" << std::endl;
    return -1;
  }
  if(quit_send == true){
    std::cout << "aborting sending file" << std::endl;
    return -1;
  }
  //Envio el nombre del fichero
  sent = netsend.send_to((void *)fname.c_str(), fname.size(), remote_address);
  if(sent != srcfile.fnamesize){
    std::cout << "Error sending initial protocol" << std::endl;
    return -1;
  }
  
  
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

    bleft -= sent;
    fptr += sent;
  }
  
  return 0;
}

//************************************************//

int receive_file(std::string outdir,std::exception_ptr& eptr) {
  // Dirección del socket remoto
  try {
    bool writeonly = true;
    char *fptr;
    sockaddr_in local_address{};	// Porque se recomienda inicializar a 0
    sockaddr_in remote_address{};

    local_address = make_ip_address(3000, "127.0.0.1");

    Socket netreceive(local_address);

    //InitMessage stprot;
    int received;
    char name[100];
 
    int fsize;
    while(true){
      if(quit_recv == true){
        std::cout << "aborting receiving file" << std::endl;
        return -1;
      }
      received = netreceive.receive_from((void *) &fsize, sizeof(int), remote_address);
      if (received == -1){
          std::cout << "Error receiving packet" << std::endl;
          return -1;
      }
      if(quit_recv == true){
        std::cout << "aborting receiving file" << std::endl;
        return -1;
      }
      received = netreceive.receive_from((void *) name, 100, remote_address);
      if (received == -1){
          std::cout << "Error receiving packet" << std::endl;
          return -1;
      }
      if(quit_recv == true){
        std::cout << "aborting receiving file" << std::endl;
        return -1;
      }
      name[received]='\0';
      std::string fname(name); 
      std::string myfile = outdir + "/" + fname; 
      
      File dest(myfile.c_str(), writeonly, fsize);

     /* netreceive.receive_from((void *) &stprot, sizeof(stprot), remote_address);
      std::string myfile = outdir + "/" + stprot.filename; 
      fsize = stprot.mess_length;
      File dest(stprot.filename.c_str(), writeonly, fsize);
      */

     // std::cout << "fichero a escribir: " << stprot.filename << std::endl;
      
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
          return -1;
        }
        received = netreceive.receive_from((void *) fptr, recsize, remote_address);
        if (received == -1){
          std::cout << "Error receiving packet" << std::endl;
          return -1;
        }
        // Primero convertimos la dirección IP como entero de 32 bits
        // en una cadena de texto.
 //       char* remote_ip = inet_ntoa(remote_address.sin_addr);

        // Recuperamos el puerto del remitente en el orden adecuado para nuestra CPU
   //     int remote_port = ntohs(remote_address.sin_port);
        bleft-=received;
        fptr+=received;
      }
    }
  }
  catch (...) {
		eptr = std::current_exception();
  }
  if (eptr) {
		std::rethrow_exception(eptr);
	}
  return 0;
}

void usr1_signal_handler(int signum)
{
    //quit_app = true;
    quit_send = true;
    quit_recv = true;
}
void usr2_signal_handler(int signum)
{
    quit_app = true;
}
void setup_signals()
{
    struct sigaction usr1_action = {0};
    struct sigaction usr2_action = {0};

    usr1_action.sa_handler = &usr1_signal_handler;
    usr2_action.sa_handler = &usr2_signal_handler;

    sigaction( SIGUSR1, &usr1_action, NULL );
    sigaction( SIGUSR2, &usr2_action, NULL );
}


void task_menu(std::exception_ptr& eptr){
	try {
    char option;
    std::string outdir, srcfile;
    std::thread recth, senth;
   // std::atomic<bool> quit_send{false};
   // std::atomic<bool> quit_recv{false};

    setup_signals();

    while(true){

      std::cout << "sigo en la thread input" << std::endl;
      std::cout << std::string(50, '\n');
      std::cout << "Please choose an option:" << std::endl;
      std::cout << "R --> Receive a file" << std::endl;
      std::cout << "S --> Send a file" << std::endl;
      std::cout << "O --> Pause an outgoing file" << std::endl;
      std::cout << "P --> Stop receiving a file" << std::endl;
      std::cout << "Q --> Quit" << std::endl;

      std::cin >> option;
      if (quit_app == true){
        std::cout << "Exiting task thread by signal" << std::endl;
        pthread_kill(recth.native_handle(), SIGUSR1);
        recth.join(); //Esperar a que termine el thread
        return;
      }

      if (option == 'R'){
        std::cout << "Please write the directory path where the file will be stored" << std::endl;
        std::cin >> outdir;
        int error = mkdir(outdir.c_str(), 0777);
        if(error == -1){
          std::cout << "Directory exists" << std::endl;
        }
        recth = std::thread(receive_file, std::ref(outdir),std::ref(eptr));
        //recth.detach();
        //std::thread receive_thread(receive_file, std::ref(outdir),std::ref(eptr),std::ref(quit_recv));
        //receive_thread.detach();
      }
      else if (option == 'S'){
        std::cout << "File name to be sent:" << std::endl;
        std::cin >> srcfile;
        senth = std::thread(send_file, std::ref(srcfile));
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
        std::cout << "señal enviada" << std::endl;
        recth.join(); 
        quit_recv = false;
        quit_send = false;
       // recth.~thread();
      }
      else if (option == 'Q'){
        std::cout << "Quitting..." << std::endl;
        //quit_recv = true;
        pthread_kill(recth.native_handle(), SIGUSR1);
        recth.join();
        //senth.join();
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
  std::thread my_thread;

  sigset_t sigwaitset;
  sigemptyset( &sigwaitset );
  sigaddset ( &sigwaitset, SIGHUP);
  sigaddset ( &sigwaitset, SIGTERM);
  sigaddset ( &sigwaitset, SIGINT);
  sigprocmask( SIG_BLOCK, &sigwaitset, NULL );
  my_thread = std::thread(task_menu, std::ref(eptr));
  //std::thread my_thread(&task_menu, std::ref(eptr));
  std::thread signals_thread(&task_signal, std::ref(eptr),std::ref(my_thread));

	// Esperar a que el hilo termine... 
	my_thread.join();
  signals_thread.join();
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

