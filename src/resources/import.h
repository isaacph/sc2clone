#ifndef RESOURCES_IMPORT_H_INCLUDED
#define RESOURCES_IMPORT_H_INCLUDED
#include <vector>
#include <string>

struct Image {
    std::string name;
    std::vector<char> data;
    int width, height;
};
struct Dependencies {
    std::vector<Image> images;
};

Dependencies loadDependencies(const std::string& resPath);

#endif