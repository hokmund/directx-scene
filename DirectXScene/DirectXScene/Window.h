#pragma once

class DirectXDevice;

class Window
{
    HINSTANCE hInst;
    HWND hWnd;
    DirectXDevice *device;

public:
    Window(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
    ~Window();
    int Start();
    HWND GetHWnd() { return hWnd; }

private:
    HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};