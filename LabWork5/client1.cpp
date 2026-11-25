// Лабораторна робота: Клієнт-серверна архітектура. Метод комунікації: поштові скриньки
// Додаток-клієнт №1

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#define ID_MAILSLOT_SEND 1

const WCHAR* MailslotName = L"\\\\.\\mailslot\\lab5"; // ім'я скриньки сервера

HWND hwndEdit;                                // текстове поле з повідомленням

// Додає текст в кінець текстового поля
void AppendText(const WCHAR* text)
{
    int len = GetWindowTextLengthW(hwndEdit);
    SendMessageW(hwndEdit, EM_SETSEL, len, len);
    SendMessageW(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
}

// Відправка тексту з текстового поля в поштову скриньку сервера
void SendToServer()
{
    WCHAR text[256];
    GetWindowTextW(hwndEdit, text, 256);

    HANDLE hMailslot = CreateFileW(MailslotName, GENERIC_WRITE, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        WCHAR error[100];
        swprintf_s(error, L"\r\nПомилка поштового серверу: %lu", GetLastError());
        AppendText(error);
        return;
    }

    DWORD size = (DWORD)((wcslen(text) + 1) * sizeof(WCHAR));
    DWORD cbWritten;
    WriteFile(hMailslot, text, size, &cbWritten, NULL);
    CloseHandle(hMailslot);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HMENU hMenu = CreateMenu();
        HMENU hSubMenu = CreatePopupMenu();
        AppendMenuW(hSubMenu, MF_STRING, ID_MAILSLOT_SEND, L"Send");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"MailSlot");
        SetMenu(hWnd, hMenu);

        // формування повідомлення за замовчуванням
        HDC hdc = GetDC(hWnd);
        WCHAR text[256];
        swprintf_s(text,
            L"Дані Клієнта #1:\r\n"
            L" - обсяг оперативної пам'яті (SM_CXVIRTUALSCREEN) = %d\r\n"
            L" - наявність зовнішнього диску (SM_REMOTECONTROL) = %d\r\n"
            L" - ширина екрану (HORZRES) = %d",
            GetSystemMetrics(SM_CXVIRTUALSCREEN),
            GetSystemMetrics(SM_REMOTECONTROL),
            GetDeviceCaps(hdc, HORZRES));
        ReleaseDC(hWnd, hdc);

        hwndEdit = CreateWindowW(L"EDIT", text,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 0, 0, hWnd, NULL, ((LPCREATESTRUCTW)lParam)->hInstance, NULL);
        return 0;
    }

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_MAILSLOT_SEND)
            SendToServer();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"MailslotClient1";
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(wc.lpszClassName, L"Клієнт 1", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 300, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
