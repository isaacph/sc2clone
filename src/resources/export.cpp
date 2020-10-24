#include <iostream>
#include <fstream>
#include <freetype2/ft2build.h>
#include <png++/png.hpp>
#include FT_FREETYPE_H

int main() {
    std::cout << "Compiling resources\n";

    const std::string basePath = "res/";
    std::vector<std::string> images;

    std::ifstream dependencies(basePath + "dependencies.txt");
    std::string line;
    while(std::getline(dependencies, line)) {
        if(line.size() > 4 && line.substr(line.size() - 4, 4) == ".png") {
            images.push_back(line);
        } else {
            std::cerr << "Unrecognized dependency: " << line << "\n";
            exit(1);
        }
    }
    dependencies.close();

    union {
        char bytes[4];
        int convert;
    } conv;

    std::ofstream out("res.dat");
    conv.convert = images.size();
    out << conv.bytes[0] << conv.bytes[1] << conv.bytes[2] << conv.bytes[3];
    
    for(const std::string& fileName : images) {
        std::cout << "Adding " << fileName << "\n";
        png::image<png::rgba_pixel> image(basePath + fileName);

        out << fileName << '\0';
        conv.convert = image.get_width();
        out << conv.bytes[0] << conv.bytes[1] << conv.bytes[2] << conv.bytes[3];
        conv.convert = image.get_height();
        out << conv.bytes[0] << conv.bytes[1] << conv.bytes[2] << conv.bytes[3];
        
        auto& buf = image.get_pixbuf();
        for(int i = 0, sz = image.get_height(); i < sz; ++i) {
            for(int j = 0, sz2 = image.get_width(); j < sz2; ++j) {
                out << buf[i][j].red << buf[i][j].green << buf[i][j].blue << buf[i][j].alpha;
            }
        }
        out.flush();
    }

    out.close();

    std::cout << "Finished compiling resources\n";

    return 0;
}