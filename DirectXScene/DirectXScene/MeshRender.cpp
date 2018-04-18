#include "MeshRender.h"
#include <fstream>
#include <string>

MeshRender::MeshRender(DirectXDevice *device)
{
    this->d3DDevice = nullptr;
    this->vertexShader = nullptr;
    this->pixelShader = nullptr;
    this->vertexLayout = nullptr;
    this->vertexBuffer = nullptr;
    this->indexBuffer = nullptr;
    this->constantBuffer = nullptr;
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
    ID3DBlob* pVSBlob = nullptr;
    auto hr = DirectXDevice::CompileShaderFromFile(L"DemoShader.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    hr = this->device->Get3dDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);

    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    const auto numElements = ARRAYSIZE(layout);

    hr = this->device->Get3dDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &this->vertexLayout);

    pVSBlob->Release();

    if (FAILED(hr))
        return hr;


    this->device->GetImmediateContext()->IASetInputLayout(this->vertexLayout);

    ID3DBlob* pPSBlob = nullptr;
    hr = DirectXDevice::CompileShaderFromFile(L"DemoShader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    hr = this->device->Get3dDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &this->pixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    auto files = 
    { 
        "Models/pokeball.obj"
    };

    for (auto file: files)
    {
        std::ifstream infile(file);

        for (std::string line; getline(infile, line); )
        {
        }
    }

    const auto vCount = 5;
    SimpleVertex vertices[vCount] =
    {
        { XMFLOAT3(4.0f,  5.0f,  4.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.0f,  0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(8.0f,  0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.0f,  0.0f,  8.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(8.0f,  0.0f,  8.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof bd);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * vCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof InitData);
    InitData.pSysMem = vertices;
    hr = this->device->Get3dDevice()->CreateBuffer(&bd, &InitData, &vertexBuffer);

    if (FAILED(hr)) return hr;

    /*WORD indices[] =
    {
        0,1,4,
        1,2,4,
        2,3,4,
        3,0,4,
        0,1,3,
        1,2,3,
    };*/

    WORD indices[] =
    {
        0,2,1,
        0,3,4,
        0,1,3,
        0,4,2,
        1,2,3,
        2,4,3,
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 18;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;

    hr = this->device->Get3dDevice()->CreateBuffer(&bd, &InitData, &indexBuffer);

    if (FAILED(hr)) return hr;

    this->device->GetImmediateContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    this->device->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = this->device->Get3dDevice()->CreateBuffer(&bd, nullptr, &this->constantBuffer);
    if (FAILED(hr))
        return hr;

    return static_cast<HRESULT>(0);
}

void MeshRender::Render(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
    auto stride = sizeof(SimpleVertex);
    UINT offset = 0;
    this->device->GetImmediateContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
    this->device->GetImmediateContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    this->device->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(*world);
    cb.mView = XMMatrixTranspose(*view);
    cb.mProjection = XMMatrixTranspose(*projection);
    this->device->GetImmediateContext()->UpdateSubresource(this->constantBuffer, 0, nullptr, &cb, 0, 0);

    this->device->GetImmediateContext()->VSSetShader(vertexShader, nullptr, 0);
    this->device->GetImmediateContext()->VSSetConstantBuffers(0, 1, &this->constantBuffer);
    this->device->GetImmediateContext()->PSSetShader(pixelShader, nullptr, 0);

    this->device->GetImmediateContext()->DrawIndexed(18, 0, 0);

    this->device->GetSwapChain()->Present(0, 0);
}