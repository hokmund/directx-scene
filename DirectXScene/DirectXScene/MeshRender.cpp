#include "MeshRender.h"


MeshRender::MeshRender(DirectXDevice *device)
{
    this->d3DDevice = nullptr;
    this->vertexShader = nullptr;
    this->pixelShader = nullptr;
    this->vertexLayout = nullptr;
    this->vertexBuffer = nullptr;
    this->indexBuffer = nullptr;
    this->constantBuffer = nullptr;
    this->countOfIndices = 0;
    this->orbit = 0.0f;
    this->device = device;
}

MeshRender::~MeshRender()
{
    if (this->constantBuffer != nullptr) this->constantBuffer->Release();
    if (this->vertexBuffer != nullptr) this->vertexBuffer->Release();
    if (this->indexBuffer != nullptr) this->indexBuffer->Release();
    if (this->vertexLayout != nullptr) this->vertexLayout->Release();
    if (this->vertexShader != nullptr) this->vertexShader->Release();
    if (this->pixelShader != nullptr) this->pixelShader->Release();
    if (this->d3DDevice != nullptr) this->d3DDevice->Release();
}

HRESULT MeshRender::InitGeometry()
{
    ID3DBlob* pVSBlob = NULL;
    HRESULT hr;
    hr = this->device->CompileShaderFromFile(L"DemoShader.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(NULL,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    hr = this->device->Get3dDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertexShader);

    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT numElements = ARRAYSIZE(layout);

    hr = this->device->Get3dDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &this->vertexLayout);

    pVSBlob->Release();

    if (FAILED(hr))
        return hr;


    this->device->GetImmediateContext()->IASetInputLayout(this->vertexLayout);

    ID3DBlob* pPSBlob = NULL;
    hr = this->device->CompileShaderFromFile(L"DemoShader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(NULL,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    hr = this->device->Get3dDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &this->pixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    SimpleVertex vertices[3];

    vertices[0].Pos.x = 0.0f;  vertices[0].Pos.y = 0.5f;  vertices[0].Pos.z = 0.5f;
    vertices[1].Pos.x = 0.5f;  vertices[1].Pos.y = -0.5f;  vertices[1].Pos.z = 0.5f;
    vertices[2].Pos.x = -0.5f;  vertices[2].Pos.y = -0.5f;  vertices[2].Pos.z = 0.5f;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = this->device->Get3dDevice()->CreateBuffer(&bd, &InitData, &vertexBuffer);

    if (FAILED(hr)) return hr;


    this->device->GetImmediateContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    this->device->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = this->device->Get3dDevice()->CreateBuffer(&bd, NULL, &this->constantBuffer);
    if (FAILED(hr))
        return hr;

    return (HRESULT)0;
}

void MeshRender::Render(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
    float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

    this->device->GetImmediateContext()->ClearRenderTargetView(this->device->GetRenderTargetView(), clearColor);
    //this->device->GetImmediateContext()->ClearDepthStencilView(this->device->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    this->device->GetImmediateContext()->VSSetShader(vertexShader, NULL, 0);
    this->device->GetImmediateContext()->PSSetShader(pixelShader, NULL, 0);

    this->device->GetImmediateContext()->Draw(3, 0);

    this->device->GetSwapChain()->Present(0, 0);
}