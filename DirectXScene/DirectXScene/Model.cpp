#include <string>
#include <fstream>
#include "MeshRender.h"
#include "Model.h"

#include <ctime>

// Split function from StackOverFlow
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        auto tStr = txt.substr(initialPos, pos - initialPos);
        
        if (tStr != "") {
            strs.push_back(tStr);
        }

        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    auto tStr = txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1);
    if (tStr != "") {
        // Add the last one.
        strs.push_back(tStr);
    }

    return strs.size();
}

float normalize(float v, float maxV, float minV) {
    return 2 * (v - minV) / (maxV - minV) - 1;
}

Model Model::LoadModel(const std::string file)
{
    auto model = Model();

    std::string line;
    std::ifstream infile(file);

    if (!infile.is_open()) {
        throw std::runtime_error("Fail");
    }

    srand((unsigned)time(0));

    while (std::getline(infile, line)) {
        auto identifier = line[0];

        switch (identifier) {
        case 'v':
            // Vertex
            if (line[1] == ' ') {
                std::vector<std::string> v;
                split(line, v, ' ');
                MeshRender::SimpleVertex vertex = {
                    XMFLOAT3(std::stof(v[1]), std::stof(v[2]), std::stof(v[3])),
                    XMFLOAT4(
                        static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.2) + 0.8,
                        static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.2) + 0.8,
                        static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.2),
                        1.0f)
                };

                model.vertices.push_back(vertex);
            }
            else {
                // Texture
            }
            break;
        case 'f':
            std::vector<std::string> v;
            split(line, v, ' ');

            if (v.size() == 4) {
                for (int i = 1; i < 4; ++i) {
                    std::vector<std::string> sInd;
                    split(v[i], sInd, '/');
                    model.indices.push_back(std::stoi(sInd[0]) - 1);
                }
            }
            else {
                // Assume it's 5.
                for (int i = 1; i < 4; ++i) {
                    std::vector<std::string> sInd;
                    split(v[i], sInd, '/');
                    model.indices.push_back(std::stoi(sInd[0]) - 1);
                }

                std::vector<std::string> sInd;
                split(v[4], sInd, '/');

                auto last = model.indices.size() - 1;
                model.indices.push_back(model.indices[last - 2]);
                model.indices.push_back(model.indices[last]);
                model.indices.push_back(std::stoi(sInd[0]) - 1);
            }

            break;
        }
    }

    auto maxX = -(float)1e6;
    auto minX = (float)1e6;

    auto maxY = -(float)1e6;
    auto minY = (float)1e6;

    auto maxZ = -(float)1e6;
    auto minZ = (float)1e6;

    for (auto v : model.vertices) {
        if (v.Pos.x > maxX) {
            maxX = v.Pos.x;
        }

        if (v.Pos.y > maxY) {
            maxY = v.Pos.y;
        }

        if (v.Pos.z > maxZ) {
            maxZ = v.Pos.z;
        }

        if (v.Pos.x < minX) {
            minX = v.Pos.x;
        }

        if (v.Pos.y < minY) {
            minY = v.Pos.y;
        }

        if (v.Pos.z < minZ) {
            minZ = v.Pos.z;
        }
    }

    for (int i = 0; i < model.vertices.size(); ++i) {
        model.vertices[i].Pos.x = normalize(model.vertices[i].Pos.x, maxX, minX);
        model.vertices[i].Pos.y = normalize(model.vertices[i].Pos.y, maxY, minY);
        model.vertices[i].Pos.z = normalize(model.vertices[i].Pos.z, maxZ, minZ);
    }

    return model;
}
