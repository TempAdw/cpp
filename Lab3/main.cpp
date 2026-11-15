#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>

static wchar_t        fileName[MAX_PATH];// ім'я файлу, яке ввів користувач
static unsigned char* fileData;// масив із вмістом файлу
static DWORD          fileSize; // розмір масиву в байтах

static long lineCount;
static long wordCount;
static long charCount;


static HANDLE fileReadEvent;
static HANDLE statsReadyEvent;

// У UTF-8 один символ може займати кілька байтів. Усі байти символу,
// крім першого, мають вигляд 10xxxxxx. Їх ми окремими символами не рахуємо.
static bool IsUtf8ContinuationByte(unsigned char c)
{
    return (c & 0xC0) == 0x80;
}


//Потік 1: читання файлу

DWORD WINAPI FileReaderThread(LPVOID)
{
    wprintf(L"Ім'я файлу: ");
    _getws_s(fileName, MAX_PATH);

    // Відкриваємо файл лише на читання
    HANDLE file = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        // Подію не встановлюємо, тому потоки 2 і 3 просто залишаються в очікуванні
        wprintf(L"Не вдалося відкрити файл. ESC - вихід\n");
        return 1;
    }

    // Виділяємо масив потрібного розміру й зчитуємо в нього весь файл
    fileSize = GetFileSize(file, NULL);
    fileData = (unsigned char*)HeapAlloc(GetProcessHeap(), 0, fileSize);
    ReadFile(file, fileData, fileSize, &fileSize, NULL);

    CloseHandle(file);// файл більше не потрібен, далі працюємо з масивом

    SetEvent(fileReadEvent);// повідомляємо потоку 2
    return 0;
}


//Потік 2: підрахунок статистики

DWORD WINAPI StatsCounterThread(LPVOID)
{
    WaitForSingleObject(fileReadEvent, INFINITE);// чекаємо, поки потік 1 прочитає файл

    bool insideWord = false;

    for (DWORD i = 0; i < fileSize; i++) {
        unsigned char c = fileData[i];

        // Рядки: кожен символ '\n' закінчує один рядок
        if (c == '\n')
            lineCount++;

        // Символи: рахуємо всі, крім байтів-продовжень UTF-8
        if (!IsUtf8ContinuationByte(c))
            charCount++;

        // Слова: слово починається там, де після пробільного символу йде непробільний
        if (isspace(c)) {
            insideWord = false;
        }
        else if (!insideWord) {
            insideWord = true;
            wordCount++;
        }
    }

    // Якщо останній рядок не закінчується на '\n', його теж треба врахувати
    if (fileSize > 0 && fileData[fileSize - 1] != '\n')
        lineCount++;

    SetEvent(statsReadyEvent);// повідомляємо потоку 3
    return 0;
}


//  Потік 3: виведення статистики

DWORD WINAPI StatsPrinterThread(LPVOID)
{
    WaitForSingleObject(statsReadyEvent, INFINITE); // чекаємо, поки потік 2 порахує

    wprintf(L"Рядків: %ld\n", lineCount);
    wprintf(L"Слів: %ld\n", wordCount);
    wprintf(L"Символів: %ld\n", charCount);
    wprintf(L"ESC - вихід\n");
    return 0;
}


int main()
{
    // Режим Unicode для консолі, щоб коректно виводилась і вводилась кирилиця
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    fileReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    statsReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Запускаємо три потоки. Потоки 2 і 3 одразу засинають на WaitForSingleObject
    CreateThread(NULL, 0, FileReaderThread, NULL, 0, NULL);
    CreateThread(NULL, 0, StatsCounterThread, NULL, 0, NULL);
    CreateThread(NULL, 0, StatsPrinterThread, NULL, 0, NULL);

    // Чекаємо на ESC. Коли main повертається, процес завершується разом з усіма потоками
    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
        Sleep(50);

    return 0;
}