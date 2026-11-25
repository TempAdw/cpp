// Лабораторна робота: Клієнт-серверна архітектура. Метод комунікації: поштові скриньки
// Додаток-сервер

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#define ID_MAILSLOT_CREATE 1
#define ID_MAILSLOT_CLOSE  2
#define ID_TIMER           1

const WCHAR* MailslotName = L"\\\\.\\mailslot\\lab5"; // ім'я скриньки

HWND   hwndEdit;                              // текстове поле для виведення даних
HANDLE hMailslot = INVALID_HANDLE_VALUE;      // дескриптор скриньки

// Додає текст в кінець текстового поля
void AppendText(const WCHAR* text)
{
    int len = GetWindowTextLengthW(hwndEdit);
    SendMessageW(hwndEdit, EM_SETSEL, len, len);
    SendMessageW(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
}

// Створення поштової скриньки та запуск таймера читання
void CreateMailslotServer(HWND hWnd)
{
    HANDLE handle = CreateMailslotW(MailslotName, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        WCHAR error[100];
        swprintf_s(error, L"Поштовий сервер. Error: %lu\r\n\r\n", GetLastError());
        AppendText(error);
        return;
    }

    hMailslot = handle;
    AppendText(L"Сервер запущено\r\n\r\n");
    SetTimer(hWnd, ID_TIMER, 1000, NULL);     // читання скриньки щосекунди
}

// Зупинка таймера та закриття поштової скриньки
void CloseMailslotServer(HWND hWnd)
{
    if (hMailslot == INVALID_HANDLE_VALUE)
        return;

    KillTimer(hWnd, ID_TIMER);
    CloseHandle(hMailslot);
    hMailslot = INVALID_HANDLE_VALUE;
    AppendText(L"Поштовий сервер закрито\r\n\r\n");
}

// Читання одного повідомлення зі скриньки (якщо воно є)
void ReadMailslot()
{
    DWORD cMessage;                           // кількість повідомлень у скриньці
    DWORD cbRead;                             // кількість прочитаних байтів
    WCHAR buffer[256];

    if (GetMailslotInfo(hMailslot, NULL, NULL, &cMessage, NULL) && cMessage != 0)
    {
        if (ReadFile(hMailslot, buffer, sizeof(buffer), &cbRead, NULL))
        {
            AppendText(buffer);
            AppendText(L"\r\n\r\n");
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HMENU hMenu = CreateMenu();
        HMENU hSubMenu = CreatePopupMenu();
        AppendMenuW(hSubMenu, MF_STRING, ID_MAILSLOT_CREATE, L"Create");
        AppendMenuW(hSubMenu, MF_STRING, ID_MAILSLOT_CLOSE, L"Close");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"MailSlot");
        SetMenu(hWnd, hMenu);

        hwndEdit = CreateWindowW(L"EDIT", L"Лабораторна робота - Поштова скриня.\r\n\r\n",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            0, 0, 0, 0, hWnd, NULL, ((LPCREATESTRUCTW)lParam)->hInstance, NULL);
        return 0;
    }

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_MAILSLOT_CREATE:
            CreateMailslotServer(hWnd);
            break;
        case ID_MAILSLOT_CLOSE:
            CloseMailslotServer(hWnd);
            break;
        }
        return 0;

    case WM_TIMER:
        if (wParam == ID_TIMER)
            ReadMailslot();
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
    wc.lpszClassName = L"MailslotServer";
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(wc.lpszClassName, L"Сервер", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 500, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
