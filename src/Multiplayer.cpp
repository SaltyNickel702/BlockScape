#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
    #define CLOSESOCKET closesocket
    #define GET_ERROR WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketType;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define CLOSESOCKET close
    #define GET_ERROR errno
#endif

#include "Multiplayer.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstring>

using namespace std;

Server::Server(){}

bool Server::open = true;
int Server::port = 25565;
std::vector<SocketType> Server::clients;

mutex Server::myMutex;
SocketType Server::serverSocket;

int Server::sendToAll(char buffer[4096]){
    if (!open) return -1;
    lock_guard<mutex> lock(myMutex);
    int success = 0;
    for (SocketType clientSocket : clients){
        if(send(clientSocket, buffer, 4096, 0) == SOCKET_ERROR) success = -1;
    }
    return success;
}

void Server::handleClient(SocketType clientSocket){
    char buffer[4096];
    while(open){
        ZeroMemory(buffer, 4096);
        for (SocketType clientSocket : clients){
            int bytesReceived = recv(clientSocket, buffer, 4096, 0);
            if (bytesReceived <= 0) {
                std::cout << "Client disconnected." << std::endl;
                clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
                closesocket(clientSocket);
                break;
            }else if(bytesReceived == SOCKET_ERROR){
                std::cout << "Client got an error and has disconnected." << std::endl;
                clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
                closesocket(clientSocket);
                break;
            } else {
                 if(sendToAll(buffer) == -1) cerr << "Error sending data to at least one client";
            }
        }
    }
}

void Server::acceptClients(){
    while(open){
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SocketType clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);//Find a client searching for your IP and socket
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }
        std::lock_guard<std::mutex> lock(myMutex); //Don't deal with more clients when you've found one
        char buffer[13];                           //Make room for their next message (hopefully it's the password)
        recv(clientSocket, buffer, 13, 0);         //Get their message
        if (buffer != "blockscape143"){ //If their message isn't our password
            closesocket(clientSocket);  //cut them off
            continue;                   //then keep listening for clients
        }
        clients.push_back(clientSocket);                  //else, add them to the vector
        std::cout << "Client connected!" << std::endl;    //Tell the server
        send(clientSocket, "Hello from server!", 18, 0);  //Tell the client
        std::thread t(handleClient, clientSocket);        //Hand them to handleClient
        t.detach();                                       //And forget about them, it's not acceptClients problem anymore
    }
}

int Server::initServer(){
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //Start the server connecter
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0); //Create the "pipe" to connect to clients
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);       //listen at port number
    serverAddr.sin_addr.s_addr = INADDR_ANY; //Let any IP connect

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { //If the serverAddr won't fit or connect, give up
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) { //build the connection to the internet
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "Server listening on port " << port << "..." << endl; //If everything works, anyone can connect to this computer with tour IP and port

    std::thread t1(acceptClients); //It's not initServer's problem anymore
    t1.detach();                   //So forget about it
    return 0;
}

Client::Client(){}

int Client::port = 25565;
string Client::serverIp = "127.0.0.1.108"; //<This assumes client is connecting to a server on the same pc

int Client::connectToServer(){
    WSADATA wsaData;
    SocketType clientSocket;
    struct sockaddr_in serverAddr;
        
    WSAStartup(MAKEWORD(2, 2), &wsaData);
        
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY; //<This is for connecting to other servers
    //serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); <This is for same computer

    cout << "Searching for " << serverIp << ":" << port << "..." << endl;

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        cerr << "Connection failed: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    } else {
        const char* message = "blockscape143";
        send(clientSocket, message, strlen(message), 0); //Send the password
        char buffer[13];
        if(recv(clientSocket, buffer, 13, 0) > 0){ //If we get a message, then we're connected now
            cout << buffer << endl;
        } else {
            cerr << "Connection failed: " << WSAGetLastError() << endl; //If we get nothing or an error, then give up
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }
    return 0;
}