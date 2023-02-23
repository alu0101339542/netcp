#include <sys/socket.h>
#include <netinet/ip.h> //Para las estructuras de sockaddr_in

class Socket {
public:
	Socket(const sockaddr_in& address);
	~Socket();

	void send_to(const Message& message, const sockaddr_in& address);
	void receive_from(Message& message, sockaddr_in& address);

private:
	int fd_;
};
