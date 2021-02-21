#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "NetworkingClient.h"

void *b; //= malloc(BUF_SIZE);
void *c1 = malloc(BUF_SIZE);
void *c2 = malloc(BUF_SIZE);
void *c3 = malloc(BUF_SIZE);
void *c4 = malloc(BUF_SIZE);
void *h = malloc(BUF_SIZE);

int numPlayers = 0;
int netSeed = 0;
int *netSeedP;

void NetworkingTest(void *str, bool flag) {
	int result;
	int socketFd;           /* UDP socket descriptor */

	/* structures for use with getaddrinfo() */
	struct addrinfo hints;  /* hints for getaddrinfo() */
	struct addrinfo *info;  /* list of info returned by getaddrinfo() */
	struct addrinfo *p;     /* pointer for iterating list in info */

	struct sockaddr_in serverAddr; /* the address of the server for sendto */

	/* indicate that we want ipv4 udp datagrams */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;      /* internet address family */
	hints.ai_socktype = SOCK_DGRAM; /* datagram sock */
	hints.ai_protocol = IPPROTO_UDP; /* udp/ip protocol */

	/* get a linked list of likely servers pointed to by info */
    result = getaddrinfo("96.236.145.9", "8026", &hints, &info);
	//result = getaddrinfo("127.0.0.1", "8026", &hints, &info);	//local host


	if (result != 0)
	{
		fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(result));
		exit(EXIT_FAILURE);
	}

	//printf("Trying...\n");
	p = info;

	while (p != NULL)
	{
		/* use current info to create a socket */
		socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if (socketFd >= 0)
		{
			/* we succeeded in getting a socket get out of this loop */
			break;
		}

		p = p->ai_next; /* try next address */
	}

	if (NULL == p)
	{
		/* we never found a server to connect to */
		fprintf(stderr, "Unable to connect to server\n.");
		freeaddrinfo(info);
		exit(EXIT_FAILURE);
	}

	/* make a copy of the server address, so we can
	 send messages to it */
	memset(&serverAddr, 0, sizeof(serverAddr));
	memcpy(&serverAddr, p->ai_addr, p->ai_addrlen);
	freeaddrinfo(info);

	/* send and receive echo messages until user sends empty message */
	// DoEchoClient(socketFd, &serverAddr, str);
	sendData(socketFd, &serverAddr, str, flag);
	receiveData(socketFd, &serverAddr, flag);

	close(socketFd);
}

void * returnReceived() {
	//printf("LENGTH OF B IN NETWORKING CLIENT: %zu\n", sizeof(b));
	return b;
}

void * returnReceivedC1(){
		return c1;
}

void * returnReceivedC2(){
		return c2;
}

void * returnReceivedC3(){
		return c3;
}

void * returnReceivedC4(){
		return c4;
}
void * returnReceivedH(){
		return h;
}

int getPlayerNumber() {
	return numPlayers;
}
int getSeed() {
	return netSeed;
}
int sendData(const int socketFd, const struct sockaddr_in *serverAddr, void *str, bool flag) {
	//printf("IN SEND DATA!\n");
	// b = malloc(BUF_SIZE);
	int result;
	struct pollfd pfds[2];  /* poll for socket recv and stdin */

	pfds[0].fd = socketFd;
	pfds[0].events = POLLIN;

	pfds[1].fd = STDIN_FILENO;
	pfds[1].events = POLLIN;
	while (1) {
		poll(pfds, 2, -1);
		/* check for recv on socket */
		/* send data to server */
		if (pfds[1].revents & POLLIN)
		{
			//printf("SENDING DATA TO SERVER\n");
			char *compare = (char*)str;
			size_t size;
			if (compare[0] == 'H' || compare[0] == 'M' || compare[0] == 'O' || compare[0] == 'S' || compare[0] == 'P') {
				size = strlen(compare + 1);
			}
			else if (compare[0] == 'C'  || compare[0] == 'R' || compare[0] == 'U' || compare[0] == 'V'){
				size = strlen(compare + 1) + 2;
			}
			else {
				size =  BUF_SIZE;
			}
			result = sendto(socketFd, str, size, 0,
			                (const struct sockaddr *)serverAddr,
			                sizeof(struct sockaddr_in));

			if (result < 0)
			{
				/* error, print error message and exit */
				perror("Error sending message to server");
				break;
			}
			else
			{
				//break out of loop because message was sent
				break;
			}
		}
	}
	// free(b);
	return result;
}

int receiveData(const int socketFd, const struct sockaddr_in *serverAddr, bool flag) {
	//printf("IN RECEIVE DATA!\n");
	b = malloc(BUF_SIZE);
	int result;
	void *buffer = malloc(BUF_SIZE); /* stores received message */
	struct pollfd pfds[2];  /* poll for socket recv and stdin */

	pfds[0].fd = socketFd;
	pfds[0].events = POLLIN;

	pfds[1].fd = STDIN_FILENO;
	pfds[1].events = POLLIN;
	while (1) {
		poll(pfds, 2, -1);
		/* check for recv on socket */
		if (pfds[0].revents & POLLIN)
		{
			//printf("RECEIVING DATA FROM SERVER\n");
			//printf("SOCKETfd IS: %d\n", socketFd);
			/* get the server's reply (recv actually accepts all replies) */
			result = recv(socketFd, buffer, BUF_SIZE, 0);
			if (result < 0)
			{
				/* receiver error, print error message and exit */
				//perror("Error receiving echo");
				// break;
			}
			else
			{
				char *buff = (char*)buffer;
				char p = buff[0];
				int pp = p - '0';
				//printf("Received bytes: %s\n", buff);
				memcpy(b, buffer, BUF_SIZE);
				if (buff[0] == 'C'){
					switch (buff[1]) {
						case '1':
							memcpy(c1, buffer, BUF_SIZE);
							break;
						case '2':
							memcpy(c2, buffer, BUF_SIZE);
							break;
						case '3':
							memcpy(c3, buffer, BUF_SIZE);
							break;
						case '4':
							memcpy(c4, buffer, BUF_SIZE);
							break;
						default:
							memcpy(c1, buffer, BUF_SIZE);

					}
				}
				else if (buff[0] == 'P') {
					// memcpy(pb, buffer, BUF_SIZE);
				}
				else if(buff[0] == 'S'){
					netSeedP = (int*)b;
					netSeed = *netSeedP;
					//printf("NETSEED IS %d\n", netSeed);
				}
				else if (buff[0] == 'U'){
					memcpy(h, buffer, BUF_SIZE);
							break;
				}
				if(pp == 1 || pp == 2 || pp == 3){
					if (flag) {
					//printf("ASSIGNING NUM PLAYERS!\n");
					numPlayers = p - '0';
					//printf("NUM PLAYERS = %d\n", numPlayers);
					}
					memset(buffer, 0, BUF_SIZE);
					break;
				}
				if(strncmp(buff, "F", 1) == 0){
					memset(b, 0, BUF_SIZE);
					memset(buffer, 0, BUF_SIZE);
					break;
				}
			}
		}
	}
	// free(b);
	free(buffer);
	return result;
}
