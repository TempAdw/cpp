#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 27015

int main()
{
    // Ініціалізація Winsock
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Створення сокета та прив'язка до порту
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(listenSocket, (sockaddr*)&addr, sizeof(addr));
    listen(listenSocket, SOMAXCONN);

    std::cout << "Server is waiting for clients on port " << PORT << "...\n\n";

    while (true)
    {
        // Прийом з'єднання від клієнта
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);

        // Прийом даних і виведення на екран
        char buffer[512];
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (received > 0)
        {
            buffer[received] = '\0';
            std::cout << buffer << "\n";
        }

        closesocket(clientSocket);
    }
}
