#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 27015

int main()
{
    // Підготовка даних
    HDC hdc = GetDC(NULL);

    char message[512];
    sprintf_s(message,
        "Client 2\n"
        "  Status bar height (SM_CYEDGE): %d\n"
        "  Message panel width (SM_CXEDGE): %d\n"
        "  Horizontal DPI (LOGPIXELSX): %d\n",
        GetSystemMetrics(SM_CYEDGE),
        GetSystemMetrics(SM_CXEDGE),
        GetDeviceCaps(hdc, LOGPIXELSX));

    ReleaseDC(NULL, hdc);

    // Ініціалізація Winsock та відкриття сокета
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // З'єднання із сервером
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cout << "Cannot connect to server\n";
        return 1;
    }

    // Відправка даних
    send(sock, message, (int)strlen(message), 0);

    closesocket(sock);
    WSACleanup();
}
