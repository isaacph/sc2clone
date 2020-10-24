#include "import.h"
#include <fstream>
#include <iostream>

Dependencies loadDependencies(const std::string& resPath) {
    Dependencies dep;
    std::ifstream res(resPath);

    union {
        char bytes[4];
        int convert;
    } conv;

    res.read(conv.bytes, 4);
    int images = conv.convert;
    dep.images.resize(images);

    for(int i = 0; i < images; ++i) {
        Image& image = dep.images[i];

        char c;
        do {
            res.read(&c, 1);
            image.name.push_back(c);
        } while(c != '\0');
        image.name.pop_back();

        res.read(conv.bytes, 4);
        image.width = conv.convert;
        res.read(conv.bytes, 4);
        image.height = conv.convert;

        image.data.resize(image.width * image.height * 4);
        res.read(image.data.data(), image.width * image.height * 4);
    }

    return dep;
}