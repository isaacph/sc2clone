//
// Created by isaac on 3/22/2020.
//

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "texture_load.h"

void TextureLibrary::bindTexture(const std::string& path, const TextureMode& params) {
    if(textures[path].find(params) == textures[path].end()) {
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
                unsigned int bytes = image->format->BytesPerPixel;
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, bytes > 3 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
                glGenerateMipmap(GL_TEXTURE_2D);
                texture_loaded = true;
            } else {
                std::cerr << "SDL Image loading error: " << path << std::endl;
            }
            //SDL_FreeSurface(image);
            delete image;
        }

        textures[path][params] = texture;
    } else {
        glBindTexture(GL_TEXTURE_2D, textures[path][params]);
    }
}

TextureLibrary::~TextureLibrary() {
    for(const auto& a : textures) {
        for(auto b : a.second) {
            glDeleteTextures(1, &b.second);
        }
    }
}