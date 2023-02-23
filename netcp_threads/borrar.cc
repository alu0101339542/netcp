void task_menu(std::exception_ptr& eptr){
	try {
    char option;
    std::string outdir, srcfile;
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
      switch(option) {
        case 'R': {
          std::cout << "Please write the directory path where the file will be stored" << std::endl;
          std::cin >> outdir;
          int error = mkdir(outdir.c_str(), 0777);
          if(error == -1){
            std::cout << "Directory exists" << std::endl;
          }
          std::thread receive_thread(receive_file, std::ref(outdir),std::ref(eptr));
          receive_thread.detach();
        }
          
          break;
        case 'S': {
          std::cout << "File name to be sent:" << std::endl;
          std::cin >> srcfile;
          std::thread send_thread(&send_file, std::ref(srcfile));
          send_thread.detach();
         }
          break;
        case 'O':
          std::cout << "Pause outgoing file" << std::endl;
          break;
        case 'P':
          std::cout << "stop receiving a file" << std::endl;
          break;
        case 'Q':
          std::cout << "Quitting..." << std::endl;
          return;
        default:
          std::cout << "Option not implemented. Please select another one" << std::endl;
          break;
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