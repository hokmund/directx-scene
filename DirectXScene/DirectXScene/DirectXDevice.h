#pragma once

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

    XMFLOAT4 light1Color;
    XMFLOAT4 light2Color;

public:
    struct Lights {
        XMFLOAT4 vLightDirs[2];
        XMFLOAT4 vLightColors[2];
    };

    DirectXDevice(Window *owner);
    ~DirectXDevice();

    HRESULT InitDevice();

    IDXGISwapChain *GetSwapChain() const { return this->swapChain; }

    ID3D11DeviceContext *GetImmediateContext() const { return this->immediateContext; }

    ID3D11Device *Get3dDevice() const { return this->d3DDevice; }

    ID3D11RenderTargetView *GetRenderTargetView() const { return renderTargetView; }

    ID3D11DepthStencilView *GetDepthStencilView() const { return depthStencilView; }

    Window *GetOwner() const { return owner; }

    static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

    XMMATRIX InitCamera();

    Lights InitLights();
};