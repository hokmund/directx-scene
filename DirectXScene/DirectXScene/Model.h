#pragma once
#include <vector>

class MeshRender;

class Model
{
public:
    struct ModelMetadata {
        std::string Path;
        XMFLOAT3 Center;
        XMFLOAT4 Color;
    };

    std::vector<MeshRender::SimpleVertex> vertices;
    std::vector<WORD> indices;
    static Model LoadModel(ModelMetadata modelMetadata);
    static XMFLOAT3 GetAdjustedCoordinates(XMFLOAT3 coordinates, ModelMetadata modelMetadata);
};
