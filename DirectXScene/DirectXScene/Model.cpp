#include <string>
#include <fstream>
#include "MeshRender.h"
#include "Model.h"


// Split function from StackOverFlow
std::size_t split(const std::string &txt, std::vector<std::string> &strs, const char ch) {
    auto pos = txt.find(ch);
    std::size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        auto tStr = txt.substr(initialPos, pos - initialPos);

        if ((ch != ' ') || !tStr.empty()) {
            strs.push_back(tStr);
        }

        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    auto tStr = txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1);
    if ((ch != ' ') || !tStr.empty()) {
        // Add the last one.
        strs.push_back(tStr);
    }

    return strs.size();
}

float normalize(const float v, const float maxV, const float minV, const float scale) {
    return (2 * (v - minV) / (maxV - minV) - 1) * scale;
}

XMFLOAT3 Model::GetAdjustedCoordinates(const XMFLOAT3 coordinates, const ModelMetadata modelMetadata) {
    return XMFLOAT3(
        coordinates.x + modelMetadata.Center.x,
        coordinates.y + modelMetadata.Center.y,
        coordinates.z + modelMetadata.Center.z
    );
}

void Model::ScaleSize(Model& model, const ModelMetadata modelMetadata)
{
    auto maxX = -static_cast<float>(1e6);
    auto minX = static_cast<float>(1e6);

    auto maxY = -static_cast<float>(1e6);
    auto minY = static_cast<float>(1e6);

    auto maxZ = -static_cast<float>(1e6);
    auto minZ = static_cast<float>(1e6);

    for (const auto v : model.vertices) {
        if (v.x > maxX) {
            maxX = v.x;
        }

        if (v.y > maxY) {
            maxY = v.y;
        }

        if (v.z > maxZ) {
            maxZ = v.z;
        }

        if (v.x < minX) {
            minX = v.x;
        }

        if (v.y < minY) {
            minY = v.y;
        }

        if (v.z < minZ) {
            minZ = v.z;
        }
    }

    for (auto& vertice : model.vertices)
    {
        vertice = GetAdjustedCoordinates(
            XMFLOAT3(
                normalize(vertice.x, maxX, minX, modelMetadata.Scale.x),
                normalize(vertice.y, maxY, minY, modelMetadata.Scale.y),
                normalize(vertice.z, maxZ, minZ, modelMetadata.Scale.z)
            ),
            modelMetadata);
    }
}

Model Model::LoadModel(const ModelMetadata modelMetadata) {
    auto model = Model();

    std::string line;
    std::ifstream infile(modelMetadata.Path);

    if (!infile.is_open()) {
        throw std::runtime_error("Fail");
    }

    srand(42);

    while (std::getline(infile, line)) {
        const auto identifier = line[0];

        switch (identifier) {
        case 'v':
            // Vertex
            if (line[1] == ' ') {
                std::vector<std::string> v;
                split(line, v, ' ');

                model.vertices.emplace_back(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
            }
            else {
                if (line[1] == 'n') {
                    std::vector<std::string> v;
                    split(line, v, ' ');

                    model.normals.emplace_back(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
                }
                else if (line[1] == 't') {
                    std::vector<std::string> v;
                    split(line, v, ' ');

                    model.textures.push_back(XMFLOAT2(std::stof(v[1]), std::stof(v[2])));
                }
            }
            break;
        case 'f':
            std::vector<std::string> v;
            split(line, v, ' ');

            if (v.size() == 4) {
                for (auto i = 1; i < 4; ++i) {
                    std::vector<std::string> sInd;
                    split(v[i], sInd, '/');
                    model.verticesIndices.push_back(std::stoi(sInd[0]) - 1);
                    model.texturesIndices.push_back(std::stoi(sInd[1]) - 1);
                    model.normalsIndices.push_back(std::stoi(sInd[2]) - 1);
                }
            }
            else {
                // Assume it's 5.
                for (int i = 1; i < 4; ++i) {
                    std::vector<std::string> sInd;
                    split(v[i], sInd, '/');
                    model.verticesIndices.push_back(std::stoi(sInd[0]) - 1);
                    model.texturesIndices.push_back(std::stoi(sInd[1]) - 1);
                    model.normalsIndices.push_back(std::stoi(sInd[2]) - 1);
                }

                std::vector<std::string> sInd;
                split(v[4], sInd, '/');

                auto last = model.verticesIndices.size() - 1;
                model.verticesIndices.push_back(model.verticesIndices[last - 2]);
                model.verticesIndices.push_back(model.verticesIndices[last]);
                model.verticesIndices.push_back(std::stoi(sInd[0]) - 1);

                last = model.texturesIndices.size() - 1;
                model.texturesIndices.push_back(model.texturesIndices[last - 2]);
                model.texturesIndices.push_back(model.texturesIndices[last]);
                model.texturesIndices.push_back(std::stoi(sInd[0]) - 1);

                last = model.normalsIndices.size() - 1;
                model.normalsIndices.push_back(model.normalsIndices[last - 2]);
                model.normalsIndices.push_back(model.normalsIndices[last]);
                model.normalsIndices.push_back(std::stoi(sInd[0]) - 1);
            }

            break;
        }
    }

    ScaleSize(model, modelMetadata);

    // If there are vertices without normal, I am fucked :(
    for (int i = 0; i < model.verticesIndices.size(); ++i) {
        const auto vertex = MeshRender::SimpleVertex {
            XMFLOAT3(
                model.vertices[model.verticesIndices[i]].x,
                model.vertices[model.verticesIndices[i]].y,
                model.vertices[model.verticesIndices[i]].z
            ),
            XMFLOAT2(
                model.textures[model.texturesIndices[i]].x,
                model.textures[model.texturesIndices[i]].y
            ),
            XMFLOAT3(
                model.normals[model.normalsIndices[i]].x,
                model.normals[model.normalsIndices[i]].y,
                model.normals[model.normalsIndices[i]].z
            )
        };

        model.verticesFinal.push_back(vertex);
        model.indicesFinal.push_back(i);
    }

    return model;
}
