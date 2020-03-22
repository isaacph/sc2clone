//
// Created by isaac on 10/6/2019.
//

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "draw.h"
#include <SDL2/SDL_image.h>

namespace draw::unique::texture {
    struct TextureResources {
        std::map<std::string, GLuint> textures;
    } res;
}

using namespace draw::unique::texture;

void draw::initTexture(const std::string& path, const draw::TextureMode& params) {
    auto ptr = res.textures.find(path);
    if(ptr != res.textures.end()) {
        return;
    }

    int width = 2;
    int height = 2;
    void* pixels = nullptr;
    GLfloat pixels_default[] = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
    };
    bool texture_loaded = false;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_mode);
    if(params.wrap_mode == GL_CLAMP_TO_BORDER) {
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, params.border_color);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.filter);

    if(!path.empty()) {
        // load texture

        SDL_Surface* image = IMG_Load(path.c_str());
        if(image != nullptr) {
            width = image->w;
            height = image->h;
            pixels = image->pixels;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            texture_loaded = true;
        }

    }

    if(!texture_loaded) {
        if(!path.empty())
            std::cerr << "Texture " << path << " failed to load!" << std::endl;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, pixels_default);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    res.textures.insert({path, texture});
}

void draw::bindTexture(const std::string &path) {
    auto ptr = res.textures.find(path);
    if(ptr == res.textures.end()) {
        if(path.empty()) {
            draw::initTexture("", {GL_NEAREST});
        }
        draw::bindTexture("");
        return;
    }
    glBindTexture(GL_TEXTURE_2D, ptr->second);
}