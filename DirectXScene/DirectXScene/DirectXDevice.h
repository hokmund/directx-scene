#pragma once

//#include "Resource.h"
#include <Windows.h>
#include <D3D11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#include "Window.h"

class DirectXDevice
{
    D3D_DRIVER_TYPE         driverType;
    D3D_FEATURE_LEVEL       featureLevel;
    ID3D11Device*           d3DDevice{};
    ID3D11DeviceContext*    immediateContext;
    IDXGISwapChain*         swapChain;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11Texture2D*        depthStencil;
    ID3D11DepthStencilView* depthStencilView;

    Window *owner;
    float orbit;
    float radius;
    float speed;

public:
    DirectXDevice(Window *owner);
    ~DirectXDevice();

    HRESULT InitDevice();

    IDXGISwapChain* GetSwapChain() { return this->swapChain; }

    ID3D11DeviceContext* GetImmediateContext() { return this->immediateContext; }

    ID3D11Device* Get3dDevice() { return this->d3DDevice; }

    ID3D11RenderTargetView* GetRenderTargetView() { return renderTargetView; }

    ID3D11DepthStencilView* GetDepthStencilView() { return depthStencilView; }

    Window *GetOwner() { return owner; }

    static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

    XMMATRIX InitCamera();
};