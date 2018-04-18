#pragma once

#include <Windows.h>
#include "DirectXDevice.h"

class MeshRender
{
    struct SimpleVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };

    struct ConstantBuffer
    {
        XMMATRIX mWorld;
        XMMATRIX mView;
        XMMATRIX mProjection;
    };

public:
    ID3D11Device*           d3DDevice;
    ID3D11VertexShader*     vertexShader;
    ID3D11PixelShader*      pixelShader;
    ID3D11InputLayout*      vertexLayout;
    ID3D11Buffer*           vertexBuffer;
    ID3D11Buffer*           indexBuffer;
    ID3D11Buffer*           constantBuffer;

    DirectXDevice *device;

    MeshRender(DirectXDevice *device);
    ~MeshRender();

    HRESULT InitGeometry();

    void Render(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
};