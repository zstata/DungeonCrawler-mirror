#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <algorithm>
#include <signal.h>
#include <sys/signalfd.h>
#include <vector>
#include <poll.h>
#include <string>
#include <string.h>
#include <math.h>
#include <utility>
#include <sstream>


#define BUF_SIZE    1024
int floorSize;
int count = 0;
int randomSeed = 0;
bool flag = true;
bool floorFlag = true;
int helloSize = 7;
int playerCount = 0;
std::vector<const sockaddr_in*> addr_list;
std::vector<int> health;
std::vector<uint32_t> s_addr;
std::vector<std::pair<int, int>> coords;
std::vector<int> spriteIndices;
std::vector<int> flips;
void* map;
bool mapFlag = false;
bool helloFlag = false;
bool coordsFlag = false;
int FloorWidth;
int FloorHeight;
int curFloorNum = 0;    //Number of current floor. Index beings at 0
int curTileType = 0;
//MAP SIZE IS VARIABLE DO NOT MAKE THESE CONSTANT AGAIN
int BG_WIDTH = 2000;    // temporary dimensions for scrolltest.png
int BG_HEIGHT = 2000;
constexpr int S_FLOOR_SIZE = 20;        //starting floor size
constexpr int F_FLOOR_SIZE = 50;        //final floor size
constexpr int NUM_FLOORS = 15;      //Total number of non-boss room floors, so actual total of floors is 1 + NUM_FLOORS

void EchoMessage(int socketFd, void* message);
int DoEcho(const int socketFd);

int AddAddr(const struct sockaddr_in* addr);
int RemoveAddr(const struct sockaddr_in* addr);


int main(int argc, char* argv[])
{
    int result;
    int socketFd;           /* server's socket descriptor */

    /* structure for echo server internet addresses */
    struct sockaddr_in serverAddr;

    /* argv[1] is port number, make sure it's passed to us */
    if (argc != 2)
    {
        fprintf(stderr, "Usage:  %s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* create a socket file descriptor for upd data */
    socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketFd < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    /* allow internet data from any address on port argv[1] */
    memset(&serverAddr, 0, sizeof(serverAddr)); /* clear data structure */
    serverAddr.sin_family = AF_INET;            /* internet addr family */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* any incoming address */
    serverAddr.sin_port = htons(atoi(argv[1])); /* port number */

    /* bind the socket to the local address */
    result = bind(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (result < 0)
    {
        /* bind failed */
        perror("Error binding socket");
        close(socketFd);
        exit(EXIT_FAILURE);
    }

    /* we have a good socket bound to a port, echo all received packets */

    DoEcho(socketFd);

    close(socketFd);

    if (result < 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
void updateFloorSize() {
    //I do not wana write out this long equation every time so just use this tiny function for now.
    FloorHeight = FloorWidth = (int)(pow((double)(curFloorNum) / (double)(NUM_FLOORS - 1), 2.0) * (double)(F_FLOOR_SIZE - S_FLOOR_SIZE)) + S_FLOOR_SIZE;
    BG_HEIGHT = BG_WIDTH = (FloorWidth * 100);
}
void EchoMessage(const int socketFd, void* message)
{
    //printf("NOW IN ECHO MESSAGE!\n");
    int result;
    std::vector<const sockaddr_in*>::iterator i;
    for (i = addr_list.begin(); i != addr_list.end(); ++i) {
        //printf("IN ECHO FOR LOOP!\n");
        size_t size;
        if (helloFlag) {
            size = 7;
        }
        else if (floorFlag) {
            size = floorSize;
        }
        else if (coordsFlag) {
            size = 17;
        }
        else {
            size = BUF_SIZE;
        }
        //printf("SIZE OF SIZE: %zu\n", size);
        result = sendto(socketFd, message, size, MSG_DONTWAIT,
            (struct sockaddr*)*(i), sizeof(struct sockaddr_in));

        if (result < 0)
        {
            if ((EAGAIN == errno) || (EWOULDBLOCK == errno))
            {
                fprintf(stderr, "Socket is busy\n");
            }
            else
            {
                /* send failed */
                perror("Error echoing message");
            }
        }
        else {
            //printf("SENT SUCCESSFULLY!\n");
        }
    }
}

int DoEcho(const int socketFd)
{
    struct sockaddr_in clientAddr;  /* address that sent the packet */
    void* buffer = malloc(BUF_SIZE);
    //char buffer[BUF_SIZE * sizeof(int)];      /* stores received message */
    int result;
    /* we'll need these to handle ctrl-c, ctrl-\ while trying to recv */
    sigset_t mask, oldMask;
    int signalFd;

    struct pollfd pfds[2];  /* poll for socket recv and signal */

    /* mask ctrl-c and ctrl-\ */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    /* block ctrl-c and ctrl-\ signals so that can be signaled
     * signaled by the signalFd and handled while polling. */
    if (sigprocmask(SIG_BLOCK, &mask, &oldMask) == -1)
    {
        perror("Error setting sigproc mask");
        return 0;
    }

    signalFd = signalfd(-1, &mask, 0);

    if (signalFd == -1)
    {
        perror("Error creating signal fd");
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        sigprocmask(SIG_BLOCK, &oldMask, NULL);
        return 0;
    }

    pfds[0].fd = socketFd;
    pfds[0].events = POLLIN;

    pfds[1].fd = signalFd;
    pfds[1].events = POLLIN;

    while (1)
    {
        /* start with cleared buffer and client address structure */
        memset(buffer, 0, BUF_SIZE);
        memset(&clientAddr, 0, sizeof(struct sockaddr_in));

        //printf("Waiting to receive a message [ctrl-c exits]:\n");
        result = 0;

        poll(pfds, 2, -1); /* block with poll until 1 of 2 events */

        /* handle signals first */
        if (pfds[1].revents & POLLIN)
        {
            /* SIGINT or SIGQUIT get out of here */
            break;
        }

        /* now check for recvfrom on socket */
        if (pfds[0].revents & POLLIN)
        {
            socklen_t addrLen = sizeof(struct sockaddr_in);

            /* use recvfrom so we can know where the data came from */
            result = recvfrom(socketFd, buffer, BUF_SIZE, 0,
                (struct sockaddr*)&clientAddr, &addrLen);

            if (result < 0)
            {
                perror("Error receiving message");
            }
            else
            {
                /* we received a valid message */
                char from[INET_ADDRSTRLEN + 1];
                from[0] = '\0';

                if (NULL !=
                    inet_ntop(AF_INET, (void*)&(clientAddr.sin_addr), from,
                        INET_ADDRSTRLEN))
                {
                    //printf("Received message from %s:%d: ", from,
                        //ntohs(clientAddr.sin_port));
                }
                else
                {
                    //printf("Received message from unresolveble address\n");
                }
                //printf("Size of buffer: %lu\n", sizeof(buffer));
                char* b = (char*)buffer;
                //printf("Size of b: %lu\n", sizeof(b));
                if (sizeof(b) > 0)
                {
                    //printf("%s\n", b);
                    int added = AddAddr(&clientAddr);

                    //int client = (int)addr_list.size();
                    int client = 2;
                    char clientC = (char)(client + '0');
                    //printf("Client Count: %d\n", client);
                    char buff[BUF_SIZE + 1];
                    buff[0] = clientC;

                    if (b[0] == 'H') {
                        floorFlag = false;
                        helloFlag = true;
                        //printf("NOW GOING TO ECHO!\n");
                        EchoMessage(socketFd, (void*)buff);
                        memset(buffer, 0, BUF_SIZE);
                        memset(b, 0, BUF_SIZE);
                    }
                    else if (b[0] == 'E') {
                        printf("RESETING SEED!\n");
                        randomSeed = 0;
                        char* f = "F";
                        EchoMessage(socketFd, f);
                        memset(buffer, 0, BUF_SIZE);
                        memset(b, 0, BUF_SIZE);
                    }
                    else if (b[0] == 'S') {
                        std::string seedS("S");
                        floorFlag = false;
                        helloFlag = true;
                        srand(time(0));
                        if (randomSeed == 0) {
                            randomSeed = rand();
                        }
                        seedS.append(std::to_string(randomSeed));
                        printf("SEED: %d\n", randomSeed);
                        EchoMessage(socketFd, (void*)seedS.c_str());

                        char* f = "F";
                        EchoMessage(socketFd, f);
                        memset(buffer, 0, BUF_SIZE);
                        memset(b, 0, BUF_SIZE);
                    }
                    else if (b[0] == 'P') {
                        floorFlag = false;
                        helloFlag = true;

                        //increase the total number of players
                        playerCount++;

                        //Send over the playerCount
                        std::string countStr("");
                        countStr.append(std::to_string(playerCount));
                        //printf("Player count currently is: %s\n", countStr.c_str());
                        EchoMessage(socketFd, (void*)countStr.c_str());

                        char* f = "F";
                        EchoMessage(socketFd, f);
                        memset(buffer, 0, BUF_SIZE);
                        memset(b, 0, BUF_SIZE);
                    }
                    else if (b[0] == 'C') {
                        floorFlag = false;
                        helloFlag = false;
                        coordsFlag = true;

                        std::string cStr(b, BUF_SIZE);
                        size_t comma_index = cStr.find(",");
                        size_t period_index = cStr.find(".");
                        size_t star_index = cStr.find("*");
                        // printf("COMMA POS = %lu\n", comma_index);

                        //Figure out the player number
                        char pNum = cStr[1];
                        int int_pNum = pNum - '0';

                        //Figure out the flip value
                        char fNum = cStr[star_index + 1];
                        int int_fNum = fNum - '0';

                        //Figure out the player's sprite index
                        std::string spriteStr = cStr.substr(2, period_index - 1);

                        // printf("This boy's number is: %d\n", int_pNum);

                        //Break message into x and y positions of this player
                        std::string xStr = cStr.substr(period_index + 1, comma_index - 1);
                        std::string yStr = cStr.substr(comma_index + 1, cStr.length() - comma_index);

                        std::stringstream streamX(xStr);
                        std::stringstream streamY(yStr);
                        std::stringstream streamSprite(spriteStr);

                        int spriteIndex = 0;
                        streamSprite >> spriteIndex;

                        int x = 0;
                        streamX >> x;
                        int y = 0;
                        streamY >> y;

                        std::pair<int, int> c_pair;
                        c_pair.first = x;
                        c_pair.second = y;

                        //If this player is new to the game, add their corrdinates to the vector
                        if (coords.size() < int_pNum) {
                            //printf("=========================================== SENDING TO BACK\n");
                            coords.push_back(c_pair);
                            spriteIndices.push_back(spriteIndex);
                            flips.push_back(int_fNum);
                        }
                        //Otherwise simply update them
                        else {
                            //printf("=========================================== OVERWRITING %d\n", int_pNum);
                            // printf("~~~~~~~~~~~~The sprite is: %d\n", spriteIndex);
                            // printf("X: %d --- Y: %d\n", x, y);
                            // printf("The flip is: %d\n", int_fNum);

                            coords[int_pNum - 1] = c_pair;
                            spriteIndices[int_pNum - 1] = spriteIndex;
                            flips[int_pNum - 1] = int_fNum;
                        }

                        // printf("X pos = %d\n", coords[int_pNum].first);
                        // printf("Y pos = %d\n", coords[int_pNum].second);

                        //Send client how many players are currently in game
                        std::string mStr("");
                        mStr.append(std::to_string((int)coords.size()));
                        const char* m = mStr.c_str();
                        //printf("sending player count %s\n", m);
                        EchoMessage(socketFd, (void*)m);

                        // std::string coordStr("1234");
                        // printf("sending test coord %s\n", coordStr.c_str());
                        // EchoMessage(socketFd, (void*)coordStr.c_str());

                        char* f = "F";
                        EchoMessage(socketFd, f);
                        memset(buffer, 0, BUF_SIZE);
                        memset(b, 0, BUF_SIZE);
                    }
                    else if (b[0] == 'U') {
                    //printf("-----------Server in 'U'--------------\n");
                    std::string hStr(b, BUF_SIZE);
                    size_t period_index = hStr.find(".");
                    //new find index
                    int i = 0;
                    std::string index = hStr.substr(1, period_index - 1);
                    std::stringstream streamH(index.c_str());

                    streamH >> i;
                    //printf("index: %d", i);
                    //Figure out health index
                    /*char num = hStr[1];
                    int index = num - '0';
                    printf("index = %d\n",index);*/
                    //get hp
                    int hp = 0;
                    std::string eHp = hStr.substr(period_index + 1, hStr.length());
                    std::stringstream streamHp(eHp.c_str());

                    streamHp >> hp;
                    //printf("before push-back\n");
                    if (health.size() == 0)
                    {
                        health.push_back(hp);
                        //printf("PUSHED\n");
                    }
                    else if (i > health.size() && i != 0)
                    {
                        health.push_back(hp);
                        //printf("PUSHED\n");
                    }
                    else
                    {
                        health[i] = hp;
                        //printf("UPDATED\n");
                    }
                    //printf("health %d\n", hp);

                    //EchoMessage(socketFd, (void*)hStr.c_str());

                    char* f = "F";
                    EchoMessage(socketFd, f);
                    memset(buffer, 0, BUF_SIZE);
                    memset(b, 0, BUF_SIZE);
                    //printf("-----------Server leaving 'U'--------------\n");
                    }
                    else if (b[0] == 'V') {
                    //printf("-----------Server entering 'V'--------------\n");
                    std::string hStr(b, BUF_SIZE);
                    //Figure out health index
                    /*char num = cStr[1];
                    int index = num - '0';
                    printf("index = %d\n",index);*/
                    size_t period_index = hStr.find(".");
                    //new find index
                    int i = 0;
                    std::string index = hStr.substr(1, period_index - 1);
                    std::stringstream streamH(index.c_str());

                    streamH >> i;
                    //printf("index: %d", i);
                    //get health from vector and send back
                    int hp = health[i];
                    std::string cStr = "U";
                    cStr.append(std::to_string(hp));

                    //printf("sending health %x\n", hp);
                    EchoMessage(socketFd, (void*)cStr.c_str());
                    //printf("-----------Server leaving 'V'--------------\n");
                    }
                    else if (b[0] == 'R') {
                        floorFlag = false;
                        helloFlag = false;
                        coordsFlag = true;

                        std::string cStr(b, BUF_SIZE);
                        //Figure out this player's number
                        char pNum = cStr[1];
                        int int_pNum = pNum - '0';

                        // std::string coordStr("1234,1234");
                        // printf("sending test coord %s\n", coordStr.c_str());
                        // EchoMessage(socketFd, (void*)coordStr.c_str());

                        //Send the player all of the other player's coordinates and sprites
                        for (int i = 0; i < coords.size(); i++) {
                            int x = coords[i].first;
                            int y = coords[i].second;
                            std::string cStr("C");
                            cStr.append(std::to_string(i + 1));
                            cStr.append(std::to_string(spriteIndices[i]));
                            cStr.append(".");
                            cStr.append(std::to_string(x));
                            cStr.append(",");
                            cStr.append(std::to_string(y));
                            cStr.append("*");
                            cStr.append(std::to_string(flips[i]));
                            //Make sure we don't send back a player's own coordinates
                            if (i != int_pNum - 1) {
                                //printf("sending coord %s\n", cStr.c_str());
                                EchoMessage(socketFd, (void*)cStr.c_str());
                            }
                        }

                        char* f = "F";
                        EchoMessage(socketFd, f);
                        memset(buffer, 0, BUF_SIZE);
                        memset(b, 0, BUF_SIZE);
                    }
                }

            }
        }
    }

    /* we should only get here by SIGINT or SIGQUIT. clean-up signal mask */
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    sigprocmask(SIG_BLOCK, &oldMask, NULL);
    close(signalFd);
    return result;
}


int AddAddr(const struct sockaddr_in* addr)
{
    //printf("ADDED ADDRESS! NEW SIZE: %d\n", (int)addr_list.size());
    /* find the end of the list checking to see if address is in the list */
    sockaddr_in addr2 = *addr;
    if (std::find(s_addr.begin(), s_addr.end(), addr->sin_addr.s_addr) != s_addr.end()) {
        //printf("ADDRESS ALREADY ADDED!\n");
        return 1;
    }
    else {
        s_addr.push_back(addr->sin_addr.s_addr);
        addr_list.push_back(addr);
        //printf("ADDED ADDRESS! NEW SIZE: %d\n", (int)addr_list.size());
    }
    return 0;
}

/*
int RemoveAddr(const struct sockaddr_in* addr)
{
    if (std::find(addr_list.begin(), addr_list.end(), addr) != addr_list.end()) {
        addr_list.erase(std::remove(addr_list.begin(), addr_list.end(), addr), addr_list.end());
        printf("REMOVED ADDRESS! NEW SIZE: %zu\n", addr_list.size());
    }
    return 0;
}
*/
