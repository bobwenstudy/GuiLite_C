#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <stdbool.h>

#include "UiBlock.h"
#include "UICode.h"

CUiBlock UIblock;

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

/* Window procedure function, handling all window messages */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // TODO: Add any drawing code that uses hdc here....
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        CUiBlock_OnLButtonDown(&UIblock, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
        CUiBlock_OnLButtonUp(&UIblock, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        CUiBlock_OnMouseMove(&UIblock, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_KEYUP:
        CUiBlock_OnKeyUp(&UIblock, wParam);
        break;
    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI ThreadCreateUI(LPVOID pParam)
{
    printf("Start uicode_create_ui! width:%d, height:%d, color_bytes:%d\n", UI_WIDTH, UI_HEIGHT, COLOR_BYTES);
    uicode_create_ui(calloc(UI_WIDTH * UI_HEIGHT, COLOR_BYTES), UI_WIDTH, UI_HEIGHT, COLOR_BYTES, NULL);
    return 0;
}

DWORD WINAPI ThreadUpdateUI(LPVOID pParam)
{
    printf("Start update_ui!\n");
    Sleep(100); // wait for host monitor initialized
    HDC hdc = GetDC((HWND)pParam);
    RECT rect;
    while (true)
    {
        GetClientRect((HWND)pParam, &rect);
        CUiBlock_renderUI(&UIblock, rect, hdc);
        Sleep(10); // per frame
    }
    return -1;
}

/* Win32 GUI program's 'main' function: This is where execution begins */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc; /* Structure for window attributes */
    HWND hwnd;     /* Handle for the window */
    MSG msg;       /* Temporary storage for messages */

    /* Initialize WNDCLASSEX structure */
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc; /* Specify the window procedure function */
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "GuiLiteWindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);   /* Load standard icon */
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* Load small icon */

    /* Register window class */
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window registration failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Make sure the window size is adjusted to the UI size
    DWORD dwExStyle = WS_EX_CLIENTEDGE;
    DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
    RECT rect = {0, 0, UI_WIDTH, UI_HEIGHT};
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
    // printf("AdjustWindowRectEx: %d, %d, %d, %d\n", rect.left, rect.top, rect.right, rect.bottom);

    char title[0x400];
    sprintf(title, "%s-%dx%d@%d", GUILITE_APP, UI_WIDTH, UI_HEIGHT, COLOR_BYTES);
    /* Create window */
    hwnd = CreateWindowEx(dwExStyle,                                      /* Extended window style */
                          "GuiLiteWindowClass",                           /* Window class name */
                          title,                                          /* Window title */
                          dwStyle,                                        /* Window style */
                          CW_USEDEFAULT, CW_USEDEFAULT,                   /* Initial x and y coordinates */
                          rect.right - rect.left, rect.bottom - rect.top, /* Window width and height */
                          NULL, NULL,                                     /* Parent window and menu */
                          hInstance,                                      /* Instance handle */
                          NULL                                            /* Additional data */
    );

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    printf("Create window success! width:%d, height:%d, color_bytes:%d\n", UI_WIDTH, UI_HEIGHT, COLOR_BYTES);

    /* Display and update window */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // create ui and update ui thread
    CUiBlock_init(&UIblock, 0, COLOR_BYTES);
    DWORD threadID;
    CreateThread(NULL, 0, ThreadCreateUI, NULL, 0, &threadID);
    CreateThread(NULL, 0, ThreadUpdateUI, hwnd, 0, &threadID);

    /* Message loop */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); /* Translate virtual-key messages into character messages */
        DispatchMessage(&msg);  /* Send message to window procedure */
    }

    /* Return exit code */
    return (int)msg.wParam;
}