#include <Windows.h>
#include "Window.h"

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine,
    const int nCmdShow)
{
    Window window(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    window.Start();

    return 0;
}
