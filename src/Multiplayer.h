#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET SocketType;
#else
    typedef int SocketType;
#endif

#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <string>
#include <mutex>
#include <vector>

class Server {
    public:
    Server();
    static bool open;
    static int port;
    int initServer();
    int sendChunk(SocketType clientSocket, char buffer[]);
    private:
    static std::mutex myMutex;
    static SocketType serverSocket;
    static std::vector<SocketType> clients;
    static int sendToAll(char buffer[]);
    static int sendToOthers(SocketType clientSocket, char buffer[]);
    static void handleClient(SocketType clientSocket);
    static void acceptClients();
};

class Client {
    public:
    Client();
    static int port;
    static std::string serverIp;
    int connectToServer();
    static SocketType clientSocket;
    private:
    void handleData();
};

bool multiplayer;
int updateChunk(int x, int y, int z, int blockID);
int sendPos(int px, int py, int pz); //Player's position
int askForChunk(int cx, int cz);

#endif