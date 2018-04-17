#pragma once

#include <Windows.h>
#include "DirectXDevice.h"

class MeshRender
{
    struct SimpleVertex
    {
        XMFLOAT3 pos;
        XMFLOAT4 color;
    };

    struct ConstantBuffer
    {
        XMMATRIX mWorld;
        XMMATRIX mView;
        XMMATRIX mProjection;
    };

public:
    struct MeshProperties
    {
        XMFLOAT3 center;
        XMFLOAT4 color;
    };

    ID3D11Device*           d3DDevice;
    ID3D11VertexShader*     vertexShader;
    ID3D11PixelShader*      pixelShader;
    ID3D11InputLayout*      vertexLayout;
    ID3D11Buffer*           vertexBuffer;
    ID3D11Buffer*           indexBuffer;
    ID3D11Buffer*           constantBuffer;

    int countOfIndices;
    float orbit;

    DirectXDevice *device;

    MeshRender(DirectXDevice *device);
    ~MeshRender();

    HRESULT InitGeometry(MeshProperties *meshArray, int countOfModels);

    void Render(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
};