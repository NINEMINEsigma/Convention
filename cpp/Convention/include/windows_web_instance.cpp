#if defined(_WIN64)||defined(_WIN32)

#include "Convention/instance/web_instance.h"


#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")



void instance<web_indicator::broadcast::server, true>::handleClient(web_indicator::socket_type clientSocket)
{
    char buffer[1024];
    while (this->get()->running) 
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) 
        {
            break;
        }
        buffer[bytesReceived] = '\0';
        broadcastMessage(buffer, clientSocket);
    }

    std::lock_guard<std::mutex> lock(this->get()->clientsMutex);
    auto it = std::find(
        this->get()->clientSockets.begin(), 
        this->get()->clientSockets.end(), 
        clientSocket);
    if (it != this->get()->clientSockets.end()) 
    {
        this->get()->clientSockets.erase(it);
    }
    closesocket(clientSocket);
}
void instance<web_indicator::broadcast::server, true>::broadcastMessage(const char* message, web_indicator::socket_type sender)
{
    std::lock_guard<std::mutex> lock(this->get()->clientsMutex);
    for (SOCKET client : this->get()->clientSockets) 
    {
        if (client != sender) 
        {
            send(client, message, strlen(message), 0);
        }
    }
}

bool instance<web_indicator::broadcast::server, true>::init() 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    this->get()->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->get()->serverSocket == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(this->get()->PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->get()->serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(this->get()->serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(this->get()->serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        closesocket(this->get()->serverSocket);
        WSACleanup();
        return false;
    }

    return true;
}
void instance<web_indicator::broadcast::server, true>::start() 
{
    this->get()->running = true;
    std::cout << "Server start，listen in " << this->get()->PORT << std::endl;

    while (this->get()->running) 
    {
        SOCKET clientSocket = accept(this->get()->serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) 
        {
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(this->get()->clientsMutex);
            this->get()->clientSockets.push_back(clientSocket);
        }

        std::thread clientThread(&instance::handleClient, this, clientSocket);
        clientThread.detach();

        std::cout << "new client link" << std::endl;
    }
}
void instance<web_indicator::broadcast::server, true>::stop() 
{
    this->get()->running = false;
    closesocket(this->get()->serverSocket);

    {
        std::lock_guard<std::mutex> lock(this->get()->clientsMutex);
        for (SOCKET client : this->get()->clientSockets) 
        {
            closesocket(client);
        }
        this->get()->clientSockets.clear();
    }

    WSACleanup();
}

void instance<web_indicator::broadcast::client, true>::receiveMessages() 
{
    char buffer[1024];
    while (this->get()->running) 
    {
        int bytesReceived = recv(this->get()->clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) 
        {
            std::cout << "The connection to the server has been dropped" << std::endl;
            this->get()->running = false;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Get: " << buffer << std::endl;
    }
}

bool instance<web_indicator::broadcast::client, true>::init() 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    this->get()->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->get()->clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(this->get()->PORT);
    inet_pton(AF_INET, this->get()->SERVER_IP, &serverAddr.sin_addr);

    if (connect(
        this->get()->clientSocket,
        (sockaddr*)&serverAddr, 
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Link failed" << std::endl;
        closesocket(this->get()->clientSocket);
        WSACleanup();
        return false;
    }

    return true;
}
void instance<web_indicator::broadcast::client, true>::start() 
{
    this->get()->running = true;
    std::cout << "Link to server" << std::endl;

    // 启动接收消息的线程
    std::thread receiveThread(&instance::receiveMessages, this);
    receiveThread.detach();

    // 主线程处理发送消息
    std::string message;
    while (this->get()->running) 
    {
        std::getline(std::cin, message);
        if (message == "exit") 
        {
            break;
        }
        if (send(this->get()->clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) 
        {
            std::cerr << "Send failed" << std::endl;
            break;
        }
    }

    stop();
}
void instance<web_indicator::broadcast::client, true>::stop() 
{
    this->get()->running = false;
    closesocket(this->get()->clientSocket);
    WSACleanup();
}

#endif // _WIN