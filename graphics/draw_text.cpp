//
// Created by isaac on 9/27/2019.
//

#include <iostream>
#include "draw.h"
#include <ft2build.h>
#include <glm/gtc/type_ptr.hpp>
#include FT_FREETYPE_H

namespace draw::unique::text {

    GLint shaderProgram;

    GLint uniformMatrix;
    GLint uniformSampler;
    GLint uniformColor;

    GLuint vao, vbo;

    FT_Library ftLibrary;

    struct Character
    {
        GLuint     TextureID;  // ID handle of the glyph texture
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        GLuint     Advance;    // Offset to advance to next glyph
    };

    struct Font
    {
        std::map<GLchar, Character> characters;
    };

    std::map<std::string, std::map<int, Font>> storedFonts;

    bool textRenderingInit = false;

    void initTextRendering();
}

using namespace draw::unique::text;

void draw::unique::text::initTextRendering() {
    if(textRenderingInit) {
        return;
    }
    textRenderingInit = true;

    std::string vsource = "ILLEGAL", fsource = "ILLEGAL";
    int posAttrib = -1, texAttrib = -1;
    try {
        vsource = draw::RESOURCES.at("shaders/text_v");
        fsource = draw::RESOURCES.at("shaders/text_f");
        posAttrib = draw::GLOBAL_ATTRIBUTE_LOCATIONS.at("position");
        texAttrib = draw::GLOBAL_ATTRIBUTE_LOCATIONS.at("texture");

        assert(posAttrib >= 0);
    } catch(std::out_of_range& err) {
        std::cout << "Resource not found! " << err.what() << std::endl;
        return;
    }

    shaderProgram = createShaderProgram(vsource, fsource, "", {"position", "texture"});
    uniformMatrix = glGetUniformLocation(shaderProgram, "matrix");
    uniformSampler = glGetUniformLocation(shaderProgram, "sampler");
    uniformColor = glGetUniformLocation(shaderProgram, "color");

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

    if (FT_Init_FreeType(&ftLibrary))
        std::cerr << "draw_text.cpp: freetype init failure" << std::endl;
}

void draw::initFont(const std::string& fontName, const int &size) {

    auto filePair = draw::RESOURCES.find("fonts");
    assert(filePair != draw::RESOURCES.end() && "draw_text.cpp: Could not find fonts path in RESOURCES map");
    std::string file = filePair->second + fontName + ".ttf";

    file = "./res/fonts/arial.ttf";

    std::cout << file << std::endl;

    initTextRendering();

    Font* font = nullptr;
    auto storedFontsPairPair = storedFonts.find(fontName);
    if(storedFontsPairPair == storedFonts.end()) {
        auto storedFontsSizeMap = storedFonts.insert({fontName, {{size, Font()}}});
        font = &storedFontsSizeMap.first->second.at(size);
        std::cout << "Font name map and size map for " << fontName << " (" << size << ")" << std::endl;
    } else {
        auto storedFontsPair = storedFontsPairPair->second.find(size);
        if(storedFontsPair == storedFontsPairPair->second.end()) {
            auto insertion = storedFontsPairPair->second.insert({size, Font()});
            font = &insertion.first->second;
            std::cout << "Font size map for " << fontName << " (" << size << ")" << std::endl;
        } else {
            font = &storedFontsPair->second;
            std::cerr << fontName << " (" << size << ") already exists. Attempted to reinitialize." << std::endl;
            return;
        }
    }

    FT_Face face;
    if (FT_New_Face(ftLibrary, file.c_str(), 0, &face))
        std::cerr << "draw_text.cpp: Could not load font file " << file << std::endl;
    FT_Set_Pixel_Sizes(face, 0, size);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        std::cerr << "draw_text.cpp: Could not load font character for " << file << std::endl;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "draw_text.cpp: Could not load font glyph " << c << std::endl;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        // Generate texture
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (GLuint) face->glyph->advance.x
        };
        font->characters.insert(std::pair<GLchar, Character>(c, character));
    }
    FT_Done_Face(face);
}

void draw::text(const std::string& fontName, const int& size, const std::string& text, const glm::mat4& matrix, const glm::vec4& color)
{
    auto storedFontsPairPair = storedFonts.find(fontName);
    if(storedFontsPairPair == storedFonts.end()) {
        std::cerr << "Font tried to draw when not defined: " << fontName << " (further undefined: " << size << ")" << std::endl;
        return;
    }
    auto storedFontsPair = storedFontsPairPair->second.find(size);
    if(storedFontsPair == storedFontsPairPair->second.end()) {
        std::cerr << "Font tried to draw when not defined: " << fontName << " (" << size << ")" << std::endl;
        return;
    }
    Font* font = &storedFontsPair->second;
    float x = 0, y = 0;
    float sx = x;
    //float ox = x;
    float scale = 1.0f;
    // Activate corresponding render state
    glUseProgram(shaderProgram);
    glUniform4f(uniformColor, color.x, color.y, color.z, color.w);
    glUniform1i(uniformSampler, 0);
    glUniformMatrix4fv(uniformMatrix, 1, false, glm::value_ptr(matrix));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        if(*c == '\n')
        {
            y += size;
            x = sx;
            continue;
        }
        Character ch = font->characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (float) (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> (GLuint) 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

float draw::textWidth(const std::string &fontName, const int &size, const std::string &text) {
    auto storedFontsPairPair = storedFonts.find(fontName);
    if(storedFontsPairPair == storedFonts.end()) {
        std::cerr << "Font tried to calc width when not defined: " << fontName << " (further undefined: " << size << ")" << std::endl;
        return 0;
    }
    auto storedFontsPair = storedFontsPairPair->second.find(size);
    if(storedFontsPair == storedFontsPairPair->second.end()) {
        std::cerr << "Font tried to calc width when not defined: " << fontName << " (" << size << ")" << std::endl;
        return 0;
    }
    Font* font = &storedFontsPair->second;
    float maxX = 0;
    float x = 0;
    float sx = x;
    float lastCharAdvance = 0, lastCharWidth = 0;
    //float ox = x;
    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        if(*c == '\n')
        {
            maxX = std::max(x - lastCharAdvance + lastCharWidth, maxX);
            x = sx;
            continue;
        }
        Character ch = font->characters[*c];

        // Update VBO for each character
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> (GLuint) 6); // Bitshift by 6 to get value in pixels (2^6 = 64)

        lastCharAdvance = (ch.Advance >> (GLuint) 6);
        lastCharWidth = ch.Size.x;
    }
    maxX = std::max(x - lastCharAdvance + lastCharWidth, maxX);
    return maxX;
}

void draw::destroyAllText() {

    FT_Done_FreeType(ftLibrary);

    for(const auto& pair1 : storedFonts) {
        for(const auto& pair2 : pair1.second) {
            for(auto character : pair2.second.characters) {
                auto c = character.second;
                glDeleteTextures(1, &c.TextureID);
            }
        }
    }

    storedFonts.clear();

    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}