#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <windows.h>

// Ім'я класу вікна
const wchar_t CLASS_NAME[] = L"Mykyta Repetuxa";

// Прототипи функцій
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawSystemMetrics(HDC hdc);

// Точка входу: реєстрація класу вікна, створення вікна, цикл повідомлень
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1. Реєстрація класу вікна
    WNDCLASSEXW wc = { 0 };
    wc.cbSize        = sizeof(WNDCLASSEXW);

    // Стиль класу: CS_DBLCLKS - вікно отримує повідомлення WM_?BUTTONDBLCLK
    // при подвійному натисканні кнопки миші, якщо курсор в межах вікна
    wc.style         = CS_DBLCLKS;

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;

    // Тип іконки - IDI_ERROR
    wc.hIcon         = LoadIconW(NULL, IDI_ERROR);
    wc.hIconSm       = LoadIconW(NULL, IDI_ERROR);

    // Тип курсору - IDC_ICON
    wc.hCursor       = LoadCursorW(NULL, IDC_ICON);

    // Колір фону вікна - чорний
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    wc.lpszClassName = CLASS_NAME;
    RegisterClassExW(&wc);

    // 2. Обчислення позиції та розмірів вікна
    // Вимога: ширина 300, висота 200 точок, вікно розташоване по нижньому краю екрана
    const int winWidth  = 300;
    const int winHeight = 200;

    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX = (screenWidth - winWidth) / 2;   // по центру горизонтально
    int posY = screenHeight - winHeight;       // впритул до нижнього краю

    // 3. Створення вікна 
    // Стиль вікна: WS_OVERLAPPEDWINDOW + WS_VSCROLL (вертикальна лінійка прокрутки)
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Лабораторна робота: системна інформація",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        posX, posY, winWidth, winHeight,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 4. Цикл обробки повідомлень
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}

// Віконна процедура - обробляє повідомлення, що надходять від ОС
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    // Малювання вмісту вікна: виводимо системну метричну інформацію
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawSystemMetrics(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    // подвійне клацання лівою кнопкою миші в межах вікна
    case WM_LBUTTONDBLCLK:
        MessageBoxW(hwnd,
            L"Отримано повідомлення WM_LBUTTONDBLCLK\n"
            L"(подвійне клацання лівої кнопки миші у межах вікна).",
            L"Повідомлення від ОС", MB_OK | MB_ICONINFORMATION);
        return 0;

    // Подвійне клацання правою кнопкою миші
    case WM_RBUTTONDBLCLK:
        MessageBoxW(hwnd,
            L"Отримано повідомлення WM_RBUTTONDBLCLK\n"
            L"(подвійне клацання правої кнопки миші у межах вікна).",
            L"Повідомлення від ОС", MB_OK | MB_ICONINFORMATION);
        return 0;

    // Обробка вертикальної лінійки прокрутки (WS_VSCROLL)
    case WM_VSCROLL:
        InvalidateRect(hwnd, NULL, TRUE); // просте оновлення вікна
        return 0;

    // Завершення роботи програми
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // Усі інші повідомлення передаються обробнику за замовчуванням
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Виведення у вікно інформації про метрику системи:
void DrawSystemMetrics(HDC hdc)
{
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SelectObject(hdc, hFont);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255)); // білий текст на чорному фоні

    int minWndWidth  = GetSystemMetrics(SM_CXMIN);       // мін ширина вікна
    int maxWndWidth  = GetSystemMetrics(SM_CXMAXTRACK);  // макс ширина вікна
    int maxIconWidth = GetSystemMetrics(SM_CXICON);      // макс ширина іконки
    int minIconWidth = GetSystemMetrics(SM_CXSMICON);    // мін ширина іконки

    wchar_t buf[128];
    int y = 15;
    const int step = 22;

    wsprintfW(buf, L"Мін. ширина вікна:   %d px", minWndWidth);
    TextOutW(hdc, 15, y, buf, lstrlenW(buf)); y += step;

    wsprintfW(buf, L"Макс. ширина вікна:  %d px", maxWndWidth);
    TextOutW(hdc, 15, y, buf, lstrlenW(buf)); y += step;

    wsprintfW(buf, L"Мін. ширина іконки:  %d px", minIconWidth);
    TextOutW(hdc, 15, y, buf, lstrlenW(buf)); y += step;

    wsprintfW(buf, L"Макс. ширина іконки: %d px", maxIconWidth);
    TextOutW(hdc, 15, y, buf, lstrlenW(buf)); y += step;
}
