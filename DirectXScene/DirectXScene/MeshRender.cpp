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

HRESULT MeshRender::InitGeometry(MeshProperties* meshArray, int countOfModels)
{
}

void MeshRender::Render(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
}
