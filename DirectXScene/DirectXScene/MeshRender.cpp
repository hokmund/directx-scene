#include <string>
#include <vector>
#include "MeshRender.h"
#include "Model.h"
#include <D3DX11tex.h>

MeshRender::MeshRender(DirectXDevice *device)
{
    this->d3DDevice = nullptr;
    this->vertexShader = nullptr;
    this->pixelShader = nullptr;
    this->vertexLayout = nullptr;
    this->vertexBuffer = nullptr;
    this->indexBuffer = nullptr;
    this->constantBuffer = nullptr;
    this->samplerLinear = nullptr;
    this->textureRV = nullptr;
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
    if (this->samplerLinear != nullptr) this->samplerLinear->Release();
    if (this->textureRV != nullptr) this->textureRV->Release();
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
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

    const auto ballsOffset = 4.0f;

    auto files = {
            Model::ModelMetadata {
            "Models/pokeball.obj",
            XMFLOAT3(-6.0f, 0.0f, ballsOffset),
            XMFLOAT3(1, 1, 1)
        },
            Model::ModelMetadata {
            "Models/pokeball.obj",
            XMFLOAT3(-3.0f, 0.0f, ballsOffset),
            XMFLOAT3(1, 1, 1)
        },
            Model::ModelMetadata {
            "Models/pokeball.obj",
            XMFLOAT3(0.0f, 0.0f, ballsOffset),
            XMFLOAT3(1, 1, 1)
        },
            Model::ModelMetadata {
            "Models/pokeball.obj",
            XMFLOAT3(3.0f, 0.0f, ballsOffset),
            XMFLOAT3(1, 1, 1)
        },
            Model::ModelMetadata {
            "Models/pokeball.obj",
            XMFLOAT3(6.0f, 0.0f, ballsOffset),
            XMFLOAT3(1, 1, 1)
        },
            Model::ModelMetadata {
            "Models/teapot.obj",
            XMFLOAT3(0.0f, 0.0f, 0.0f),
            XMFLOAT3(2, 2, 2)
        },
            Model::ModelMetadata{
            "Models/cube.obj",
            XMFLOAT3(0.0f, -2.0f, 0.0f),
            XMFLOAT3(8, 1, 8)
        },
    };

    std::vector<SimpleVertex> vertices;
    std::vector<WORD> indices;
    for (const auto& file : files) {
        auto model = Model::LoadModel(file);

        // Indices of latter objects should start from bigger numbers.
        for (auto& i : model.indicesFinal) {
            i += vertices.size();
        }

        vertices.insert(vertices.end(), model.verticesFinal.begin(), model.verticesFinal.end());
        indices.insert(indices.end(), model.indicesFinal.begin(), model.indicesFinal.end());
    }

    this->verticesSize = vertices.size();
    this->indicesSize = indices.size();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof bd);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * this->verticesSize;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof InitData);
    InitData.pSysMem = vertices.data();
    hr = this->device->Get3dDevice()->CreateBuffer(&bd, &InitData, &vertexBuffer);

    if (FAILED(hr)) return hr;

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * this->indicesSize;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices.data();

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

    // Textures
    hr = D3DX11CreateShaderResourceViewFromFile(this->device->Get3dDevice(), L"Textures/concrete.jpg",
        nullptr, nullptr, &textureRV, nullptr);

    if (FAILED(hr)) return hr;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = this->device->Get3dDevice()->CreateSamplerState(&sampDesc, &samplerLinear);

    if (FAILED(hr)) {
        return hr;
    }

    return static_cast<HRESULT>(0);
}

void MeshRender::Render(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, DirectXDevice::Lights lights) const
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
    cb.vLightDirs[0] = lights.vLightDirs[0];
    cb.vLightColors[0] = lights.vLightColors[0];

    cb.vLightDirs[1] = lights.vLightDirs[1];
    cb.vLightColors[1] = lights.vLightColors[1];

    this->device->GetImmediateContext()->UpdateSubresource(this->constantBuffer, 0, nullptr, &cb, 0, 0);

    this->device->GetImmediateContext()->VSSetShader(vertexShader, nullptr, 0);
    this->device->GetImmediateContext()->VSSetConstantBuffers(0, 1, &this->constantBuffer);
    this->device->GetImmediateContext()->PSSetShader(pixelShader, nullptr, 0);
    this->device->GetImmediateContext()->PSSetConstantBuffers(0, 1, &this->constantBuffer);
    this->device->GetImmediateContext()->PSSetShaderResources(0, 1, &this->textureRV);
    this->device->GetImmediateContext()->PSSetSamplers(0, 1, &samplerLinear);

    this->device->GetImmediateContext()->DrawIndexed(this->indicesSize, 0, 0);

    this->device->GetSwapChain()->Present(0, 0);
}
