#include "framework.h"
#include "11.h"
#include <windows.h>
#include <CommCtrl.h>
#include <Commdlg.h>
#define MAX_LOADSTRING 100
#define IDC_MONTHCAL 111    
#define IDC_IPADDRESS 11
#define IDC_SLIDER 113
#define IDC_LISTBOX 114
#define IDC_SAVEAS 115
#define IDC_FIND 116
#define ON_EVENTS 117
#define MAX_FIND_TEXT 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Добавленные глобальные переменные:
FINDREPLACE findDlg = { sizeof(FINDREPLACE) };
HWND hFindDialog = NULL;
WCHAR szFindBuffer[MAX_FIND_TEXT] = { 0 };
bool events = false;  // Глобальная переменная для хранения состояния событий MouseMove

// Отправить объявления функций, включенных в этот модуль кода:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY11, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY11));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY11));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY11);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }


    // Month Calendar
    CreateWindowEx(0, MONTHCAL_CLASS, NULL,
        WS_BORDER | WS_CHILD | WS_VISIBLE | MCS_DAYSTATE,
        10, 10, 200, 170,
        hWnd, (HMENU)IDC_MONTHCAL, hInstance, NULL);

    // Поле для ввода IP-адреса
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        10, 190, 200, 25,
        hWnd, (HMENU)IDC_IPADDRESS, hInstance, NULL);

    // Регулятор
    CreateWindowEx(0, TRACKBAR_CLASS, NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        10, 240, 200, 30,
        hWnd, (HMENU)IDC_SLIDER, hInstance, NULL);

    // ListBox
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
        10, 300, 200, 100,
        hWnd, (HMENU)IDC_LISTBOX, hInstance, NULL);

    // Кнопка "Сохранить как"
    CreateWindowEx(0, L"BUTTON", L"Сохранить как",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        220, 10, 200, 30, hWnd, (HMENU)IDC_SAVEAS, hInstance, NULL);

    // Кнопка "Найти"
    CreateWindowEx(0, L"BUTTON", L"Найти",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        220, 50, 200, 30, hWnd, (HMENU)IDC_FIND, hInstance, NULL);

    // Кнопка "Включить MouseMove"
    CreateWindowEx(0, L"BUTTON", L"Включить MouseMove",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        220, 90, 200, 30, hWnd, (HMENU)ON_EVENTS, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDC_SAVEAS:
        {
            OPENFILENAME ofn;
            TCHAR szFile[MAX_PATH] = L"";

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szFile;
            ofn.lpstrDefExt = L"txt";
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn) == TRUE)
            {
            }
        }
        break;
        case IDC_FIND:
        {
            if (hFindDialog == NULL) {
                findDlg.hwndOwner = hWnd;
                findDlg.lpstrFindWhat = szFindBuffer;
                findDlg.wFindWhatLen = MAX_FIND_TEXT;
                hFindDialog = FindText(&findDlg);
            }
            else {
                SetFocus(hFindDialog);
            }
        }
        break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case ON_EVENTS:
        {
            events = true;
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CHAR:
    {
        MessageBox(hWnd, L"WM_CHAR", L"Событие WM_CHAR", MB_OK | MB_ICONINFORMATION);
    }
    break;
    case WM_KEYDOWN:
    {
        MessageBox(hWnd, L"WM_KEYDOWN", L"WM_KEYDOWN", MB_OK | MB_ICONINFORMATION);
    }
    break;
    case WM_MOUSEMOVE:
    {
        if (events)
        {
            MessageBox(hWnd, L"WM_MOUSEMOVE", L"WM_MOUSEMOVE", MB_OK | MB_ICONINFORMATION);
        }
    }
    break;
    case WM_MOVE:
    {
        MessageBox(hWnd, L"WM_MOVE", L"WM_MOVE", MB_OK | MB_ICONINFORMATION);
    }
    break;
    case WM_RBUTTONDBLCLK:
    {
        MessageBox(hWnd, L"RBUTTONDBLCLK", L"Событие RBUTTONDBLCLK", MB_OK | MB_ICONINFORMATION);
    }
    break;

    case WM_RBUTTONDOWN:
    {
        MessageBox(hWnd, L"RBUTTONDOWN", L"Событие RBUTTONDOWN", MB_OK | MB_ICONINFORMATION);
    }
    break;

    case WM_RBUTTONUP:
    {
        MessageBox(hWnd, L"RBUTTONUP", L"Событие RBUTTONUP", MB_OK | MB_ICONINFORMATION);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
