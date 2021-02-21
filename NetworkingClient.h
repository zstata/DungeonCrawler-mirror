#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

#define BUF_SIZE    40000
int sendData(const int socketFd, const struct sockaddr_in *serverAddr, void *str, bool flag);
int receiveData(const int socketFd, const struct sockaddr_in *serverAddr, bool flag);
void NetworkingTest(void *str, bool flag);
int getPlayerNumber();
int getSeed();
void * returnReceived();
void * returnReceivedC1();
void * returnReceivedC2();
void * returnReceivedC3();
void * returnReceivedC4();
void * returnReceivedG();
void * returnReceivedH();