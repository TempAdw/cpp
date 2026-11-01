#include <windows.h>

// Заливає область, якій належить точка (x, y).
// Заливка розповсюджується від цієї точки в усі боки
// і зупиняється, коли натрапляє на чорний контур фігури.
void FillArea(HDC hdc, int x, int y, int hatchStyle, COLORREF color)
{
    HBRUSH brush = CreateHatchBrush(hatchStyle, color);   // штрихування + колір
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    ExtFloodFill(hdc, x, y, RGB(0, 0, 0), FLOODFILLBORDER);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

void DrawScene(HDC hdc)
{
    // 1. Малюємо тільки контури: чорне перо і порожній пензель
    SelectObject(hdc, GetStockObject(BLACK_PEN));
    SelectObject(hdc, GetStockObject(NULL_BRUSH));

    // Два прямокутники
    Rectangle(hdc, 30, 30, 230, 180);
    Rectangle(hdc, 30, 220, 230, 370);

    // Два еліпси
    Ellipse(hdc, 300, 30, 500, 180);
    Ellipse(hdc, 300, 220, 500, 370);

    // Два полігони: трикутник і п'ятикутник
    POINT triangle[3] = { {660, 30}, {760, 180}, {560, 180} };
    Polygon(hdc, triangle, 3);

    POINT pentagon[5] = { {660, 220}, {760, 290}, {720, 370}, {600, 370}, {560, 290} };
    Polygon(hdc, pentagon, 5);

    // 2. Заливаємо кожну фігуру: точка всередині, стиль штрихування, колір
    FillArea(hdc, 130, 105, HS_HORIZONTAL, RGB(255, 0, 0));      // прямокутник 1
    FillArea(hdc, 130, 295, HS_VERTICAL,   RGB(0, 160, 0));      // прямокутник 2

    FillArea(hdc, 400, 105, HS_FDIAGONAL,  RGB(0, 0, 255));      // еліпс 1
    FillArea(hdc, 400, 295, HS_BDIAGONAL,  RGB(255, 128, 0));    // еліпс 2

    FillArea(hdc, 660, 130, HS_CROSS,      RGB(160, 0, 160));    // трикутник
    FillArea(hdc, 660, 300, HS_DIAGCROSS,  RGB(0, 128, 128));    // п'ятикутник
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawScene(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);   // білий фон
    wc.lpszClassName = L"FillWindow";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(L"FillWindow", L"Flood fill",
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 820, 450,
                              NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
