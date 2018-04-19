#pragma once
#include <vector>

class MeshRender;

class Model
{
public:
    struct ModelMetadata {
        std::string Path;
        XMFLOAT3 Center;
        XMFLOAT3 Scale;
    };

    std::vector<MeshRender::SimpleVertex> verticesFinal;
    std::vector<WORD> indicesFinal;

    std::vector<XMFLOAT3> vertices;
    std::vector<XMFLOAT3> normals;
    std::vector<XMFLOAT2> textures;
    std::vector<WORD> verticesIndices;
    std::vector<WORD> normalsIndices;
    std::vector<WORD> texturesIndices;
    static void ScaleSize(Model& model, ModelMetadata modelMetadata);
    static Model LoadModel(ModelMetadata modelMetadata);
    static XMFLOAT3 GetAdjustedCoordinates(XMFLOAT3 coordinates, ModelMetadata modelMetadata);
};
