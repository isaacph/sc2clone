//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_TEXTURE_LOAD_H
#define UNTITLED2_TEXTURE_LOAD_H

#include <GL/glew.h>
#include <map>
#include "constants.h"

// default: GL_NEAREST, GL_CLAMP_TO_BORDER, {0,0,0,0} border color
struct TextureMode {
    // can be GL_NEAREST,
    // GL_LINEAR,
    // GL_NEAREST_MIPMAP_NEAREST,
    // GL_LINEAR_MIPMAP_NEAREST,
    // GL_NEAREST_MIPMAP_LINEAR,
    // GL_LINEAR_MIPMAP_LINEAR
    // description of mipmap params from https://open.gl/textures
    // GL_NEAREST_MIPMAP_NEAREST: Uses the mipmap that most closely matches the size of the pixel being textured
    //      and samples with nearest neighbour interpolation.
    // GL_LINEAR_MIPMAP_NEAREST: Samples the closest mipmap with linear interpolation.
    // GL_NEAREST_MIPMAP_LINEAR: Uses the two mipmaps that most closely match the size of the pixel being textured
    //      and samples with nearest neighbour interpolation.
    // GL_LINEAR_MIPMAP_LINEAR: Samples closest two mipmaps with linear interpolation.
    GLuint filter = GL_NEAREST;

    // can be GL_CLAMP_TO_BORDER, GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE
    GLuint wrap_mode = GL_CLAMP_TO_BORDER;

    // used if wrap_mode is GL_CLAMP_TO_BORDER to define the border's color
    float border_color[4] = {0, 0, 0, 0};

    inline bool operator<(const TextureMode& other) const {
        return filter == other.filter ? wrap_mode == other.wrap_mode ?
            border_color[0] == other.border_color[0] ?
                border_color[1] == other.border_color[1] ?
                    border_color[2] == other.border_color[2] ?
                        border_color[3] == other.border_color[3] ? false : border_color[3] < other.border_color[3]
                    : border_color[2] < other.border_color[2]
                : border_color[1] < other.border_color[1]
            : border_color[0] < other.border_color[0]
        : wrap_mode < other.wrap_mode : filter < other.filter;
    }
};

class TextureLibrary {
public:
    void bindTexture(const std::string& path, const TextureMode& params);
    ~TextureLibrary();
private:
    std::map<std::string, std::map<TextureMode, GLuint>> textures;
};

#endif //UNTITLED2_TEXTURE_LOAD_H
