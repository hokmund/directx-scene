#include "DirectXDevice.h"
#include <D3DX11async.h>
#include <cmath>

DirectXDevice::DirectXDevice(Window *owner)
{
    this->driverType = D3D_DRIVER_TYPE_NULL;
    this->featureLevel = D3D_FEATURE_LEVEL_11_0;
    this->immediateContext = nullptr;
    this->swapChain = nullptr;
    this->renderTargetView = nullptr;
    this->depthStencil = nullptr;
    this->depthStencilView = nullptr;

    this->orbit = 0.0f;
    this->radius = 10.0f;
    this->speed = 0.00015f;

    this->owner = owner;

    this->light1Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.3f);
    this->light2Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.3f);

    this->light1Color = XMFLOAT4(1.0f, 0.f, 0.f, 0.3f);
    this->light2Color = XMFLOAT4(0.f, 0.f, 1.0f, 0.3f);
}

DirectXDevice::~DirectXDevice()
{
    if (this->immediateContext != nullptr) this->immediateContext->ClearState();
    if (this->renderTargetView != nullptr) this->renderTargetView->Release();
    if (this->swapChain != nullptr) this->swapChain->Release();
    if (this->immediateContext != nullptr) this->immediateContext->Release();
}

HRESULT DirectXDevice::InitDevice()
{
    auto hr = S_OK;

    RECT rc;
    GetClientRect(this->owner->GetHWnd(), &rc);
    const UINT width = rc.right - rc.left;
    const UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    const auto numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof sd);
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 80;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = owner->GetHWnd();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (auto& driverType : driverTypes)
    {
        this->driverType = driverType;
        hr = D3D11CreateDeviceAndSwapChain(nullptr, this->driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &this->swapChain, &this->d3DDevice, &this->featureLevel, &this->immediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = this->d3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &this->renderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof descDepth);
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = this->d3DDevice->CreateTexture2D(&descDepth, nullptr, &this->depthStencil);
    if (FAILED(hr))
        return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof descDSV);
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = this->d3DDevice->CreateDepthStencilView(this->depthStencil, &descDSV, &this->depthStencilView);
    if (FAILED(hr))
        return hr;

    this->immediateContext->OMSetRenderTargets(1, &this->renderTargetView, this->depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    this->immediateContext->RSSetViewports(1, &vp);

    return S_OK;
}

XMMATRIX DirectXDevice::InitCamera()
{
    float clearColor[4] = { 0.53f, 0.80f, 0.98f, 1.0f };
    this->immediateContext->ClearRenderTargetView(this->renderTargetView, clearColor);
    this->immediateContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    this->orbit += this->speed;
    const auto eye = XMVectorSet(std::sin(this->orbit)*this->radius, 2.0f, std::cos(this->orbit)*this->radius, 1.0f);
    const auto at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    const auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(eye, at, up);
}

float adjustColorComponent(const float component, const float randPart) {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX / randPart) + component - randPart / 2;
}

XMFLOAT4 adjustColor(const XMFLOAT4 color) {
    const auto randPart = 0.005;

    const auto x = adjustColorComponent(color.x, randPart);
    const auto y = adjustColorComponent(color.y, randPart);
    const auto z = adjustColorComponent(color.z, randPart);
    const auto w = adjustColorComponent(color.w, randPart);

    return XMFLOAT4(
        max(min(x, 1), 0),
        max(min(y, 1), 0),
        max(min(z, 1), 0),
        max(min(w, 1), 0)
    );
}

DirectXDevice::Lights DirectXDevice::InitLights()
{
    //this->light1Color = adjustColor(this->light1Color);
    this->light2Color = adjustColor(this->light2Color);

    return Lights{
        { XMFLOAT4(8.0f, 6.0f, 0.8f, 1.0f), XMFLOAT4(-8.0f, 6.0f, 0.5f, 1.0f) },
        { light1Color, light2Color },
    };
}

HRESULT DirectXDevice::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, nullptr, ppBlobOut, &pErrorBlob, nullptr);
    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA(static_cast<char*>(pErrorBlob->GetBufferPointer()));
        if (pErrorBlob) pErrorBlob->Release();
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}
