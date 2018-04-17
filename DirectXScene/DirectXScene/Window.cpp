#include <Windows.h>
#include <ctime>

#include "Window.h"
#include "DirectXDevice.h"
#include "resource.h"

Window::Window(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    this->hInst = nullptr;
    this->hWnd = nullptr;
    srand(static_cast<unsigned int>(time(nullptr)));

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (FAILED(this->InitWindow(hInstance, nCmdShow)))
        return;

    this->device = new DirectXDevice(this);

    if (FAILED(this->device->InitDevice()))
    {
        return;
    }
}


Window::~Window()
{
    delete this->device;
}

int Window::Start()
{
    MSG msg = { nullptr };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            RECT rc;
            GetClientRect(this->hWnd, &rc);
            const UINT width = rc.right - rc.left;
            const UINT height = rc.bottom - rc.top;
            auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / static_cast<FLOAT>(height), 0.01f, 1000.0f);
        }
    }

    return static_cast<int>(msg.wParam);
}

HRESULT Window::InitWindow(HINSTANCE hInstance, const int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"Window";
    wcex.hIconSm = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_ICON1));
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    this->hInst = hInstance;
    RECT rc = { 0, 0, 533, 400 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    this->hWnd = CreateWindow(
        L"Window",
        L"I <3 DirectX",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!this->hWnd)
        return E_FAIL;

    ShowWindow(this->hWnd, nCmdShow);

    return S_OK;
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
