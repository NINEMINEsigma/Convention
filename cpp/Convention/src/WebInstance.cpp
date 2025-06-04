
#include "Convention/instance/WebInstance.h"

#if defined(_WIN64)||defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif // _WIN

using namespace std;
using namespace Convention;

template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Server, true, Allocator, false>::HandleClient(WebIndicator::SocketType clientSocket)
{
    char buffer[1024];
    while (this->ReadValue().running) 
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) 
        {
            break;
        }
        buffer[bytesReceived] = '\0';
        BroadcastMessage(buffer, clientSocket);
    }

    std::lock_guard<std::mutex> lock(this->ReadValue().clientsMutex);
    auto it = std::find(
        this->ReadValue().clientSockets.begin(), 
        this->ReadValue().clientSockets.end(), 
        clientSocket);
    if (it != this->ReadValue().clientSockets.end()) 
    {
        this->ReadValue().clientSockets.erase(it);
    }
    closesocket(clientSocket);
}
template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Server, true, Allocator, false>::BroadcastMessage(const char* message, WebIndicator::SocketType sender)
{
    std::lock_guard<std::mutex> lock(this->ReadValue().clientsMutex);
    for (SOCKET client : this->ReadValue().clientSockets) 
    {
        if (client != sender) 
        {
            send(client, message, strlen(message), 0);
        }
    }
}

template<template<typename...> class Allocator>
bool instance<WebIndicator::Broadcast::Server, true, Allocator, false>::Init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    this->ReadValue().serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->ReadValue().serverSocket == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(this->ReadValue().PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->ReadValue().serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(this->ReadValue().serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(this->ReadValue().serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        closesocket(this->ReadValue().serverSocket);
        WSACleanup();
        return false;
    }

    return true;
}
template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Server, true, Allocator, false>::Start()
{
    this->ReadValue().running = true;
    std::cout << "Server start，listen in " << this->ReadValue().PORT << std::endl;

    while (this->ReadValue().running) 
    {
        SOCKET clientSocket = accept(this->ReadValue().serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) 
        {
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(this->ReadValue().clientsMutex);
            this->ReadValue().clientSockets.push_back(clientSocket);
        }

        std::thread clientThread(&instance::HandleClient, this, clientSocket);
        clientThread.detach();

        std::cout << "new client link" << std::endl;
    }
}
template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Server, true, Allocator, false>::Stop()
{
    this->ReadValue().running = false;
    closesocket(this->ReadValue().serverSocket);

    {
        std::lock_guard<std::mutex> lock(this->ReadValue().clientsMutex);
        for (SOCKET client : this->ReadValue().clientSockets)
        {
            closesocket(client);
        }
        this->ReadValue().clientSockets.clear();
    }

    WSACleanup();
}

template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Client, true, Allocator, false>::ReceiveMessages()
{
    char buffer[1024];
    while (this->ReadValue().running)
    {
        int bytesReceived = recv(this->ReadValue().clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) 
        {
            std::cout << "The connection to the Server has been dropped" << std::endl;
            this->ReadValue().running = false;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Get: " << buffer << std::endl;
    }
}

template<template<typename...> class Allocator>
bool instance<WebIndicator::Broadcast::Client, true, Allocator, false>::Init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    this->ReadValue().clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->ReadValue().clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(this->ReadValue().PORT);
    inet_pton(AF_INET, this->ReadValue().SERVER_IP, &serverAddr.sin_addr);

    if (connect(
        this->ReadValue().clientSocket,
        (sockaddr*)&serverAddr, 
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Link failed" << std::endl;
        closesocket(this->ReadValue().clientSocket);
        WSACleanup();
        return false;
    }

    return true;
}
template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Client, true, Allocator, false>::Start()
{
    this->ReadValue().running = true;
    std::cout << "Link to Server" << std::endl;

    // 启动接收消息的线程
    std::thread receiveThread(&instance::ReceiveMessages, this);
    receiveThread.detach();

    // 主线程处理发送消息
    std::string message;
    while (this->ReadValue().running) 
    {
        std::getline(std::cin, message);
        if (message == "exit") 
        {
            break;
        }
        if (send(this->ReadValue().clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR)
        {
            std::cerr << "Send failed" << std::endl;
            break;
        }
    }

    Stop();
}
template<template<typename...> class Allocator>
void instance<WebIndicator::Broadcast::Client, true, Allocator, false>::Stop()
{
    this->ReadValue().running = false;
    closesocket(this->ReadValue().clientSocket);
    WSACleanup();
}
