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
    int sendChunk(char buffer[4095]);
    private:
    static std::mutex myMutex;
    static SocketType serverSocket;
    static std::vector<SocketType> clients;
    static int sendToAll(char buffer[4096]);
    static void handleClient(SocketType clientSocket);
    static void acceptClients();
};

class Client {
    public:
    Client();
    static int port;
    static std::string serverIp;
    int connectToServer();
    int updateChunk(char buffer[4095]);
    int sendPos(char buffer[4095]); //Player position
    private:
    static SocketType clientSocket;
};

#endif