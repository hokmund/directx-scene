#pragma once
#include <vector>
#include "MeshRender.h"

class Model
{
public:
    std::vector<MeshRender::SimpleVertex> vertices;
    std::vector<WORD> indices;
    static Model LoadModel(std::string file);
};
