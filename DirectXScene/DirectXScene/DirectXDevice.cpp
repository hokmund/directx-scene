#include "DirectXDevice.h"
#include <d3dx11async.h>

DirectXDevice::DirectXDevice(Window *owner)
{
    this->driverType = D3D_DRIVER_TYPE_NULL;
    this->featureLevel = D3D_FEATURE_LEVEL_11_0;
    this->immediateContext = NULL;
    this->swapChain = NULL;
    this->renderTargetView = NULL;
    this->depthStencil = NULL;
    this->depthStencilView = NULL;

    this->orbit = 0.0f;
    this->radius = 40.0f;
    this->speed = 0.0001f;

    this->owner = owner;
}

DirectXDevice::~DirectXDevice()
{
    if (nullptr != this->immediateContext) this->immediateContext->ClearState();
    if (nullptr != this->renderTargetView) this->renderTargetView->Release();
    if (nullptr != this->swapChain) this->swapChain->Release();
    if (nullptr != this->immediateContext) this->immediateContext->Release();
}

HRESULT DirectXDevice::InitDevice()
{
    HRESULT hr = S_OK;

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
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = owner->GetHWnd();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        this->driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(NULL, this->driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &this->swapChain, &this->d3dDevice, &this->featureLevel, &this->immediateContext);
        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    ID3D11Texture2D* pBackBuffer = NULL;
    hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;

    hr = this->d3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &this->renderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
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
    hr = this->d3dDevice->CreateTexture2D(&descDepth, NULL, &this->depthStencil);
    if (FAILED(hr))
        return hr;

    // Создание z-буфреа
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = this->d3dDevice->CreateDepthStencilView(this->depthStencil, &descDSV, &this->depthStencilView);
    if (FAILED(hr))
        return hr;

    this->immediateContext->OMSetRenderTargets(1, &this->renderTargetView, this->depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    this->immediateContext->RSSetViewports(1, &vp);

    //Выключаем backface culling
    /*	D3D11_RASTERIZER_DESC rasterDesc;

    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    ID3D11RasterizerState* m_rasterStateNoCulling;

    HRESULT result = this->d3dDevice->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
    if (FAILED(result))
    {
    return false;
    }

    this->immediateContext->RSSetState(m_rasterStateNoCulling);
    */


    return S_OK;
}

XMMATRIX DirectXDevice::InitCamera()
{
    float ClearColor[4] = { 0.0f, 0.9f, 0.5f, 1.0f };
    this->immediateContext->ClearRenderTargetView(this->renderTargetView, ClearColor);
    this->immediateContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    this->orbit += this->speed;
    XMVECTOR Eye = XMVectorSet(sin(this->orbit)*this->radius, 2.0f, cos(this->orbit)*this->radius, 1.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    return XMMatrixLookAtLH(Eye, At, Up);
}

HRESULT DirectXDevice::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    auto hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
    if (FAILED(hr))
    {
        if (pErrorBlob != NULL)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        if (pErrorBlob) pErrorBlob->Release();
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}
