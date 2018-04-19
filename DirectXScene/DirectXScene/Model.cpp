#include "Model.h"
#include <string>
#include <fstream>

Model Model::LoadModel(const std::string file)
{
    auto model = Model();

    std::string line;
    std::ifstream infile(file);

    if (!infile.is_open())
    {
        throw std::runtime_error("Fail");
    }

    while (std::getline(infile, line))
    {
    }

    return model;
}
