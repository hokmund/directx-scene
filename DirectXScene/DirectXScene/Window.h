#pragma once

class MeshRender;
class DirectXDevice;

class Window
{
    HINSTANCE hInst;
    HWND hWnd;
    DirectXDevice *device;
    MeshRender *meshRender;

public:
    Window(HINSTANCE hInstance, HINSTANCE hPrevInstance, const LPWSTR lpCmdLine, int nCmdShow);
    ~Window();
    int Start() const;
    HWND GetHWnd() const { return hWnd; }

private:
    HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};