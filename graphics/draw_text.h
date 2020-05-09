//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_DRAW_TEXT_H
#define UNTITLED2_DRAW_TEXT_H
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "constants.h"

class DrawText {
public:
    DrawText();
    void initFont(const std::string& name, int size);
    void drawFont(glm::mat4 matrix, glm::vec4 color, const std::string& name, int size, std::string text, bool allowNewLines);
    float textWidth(const std::string& name, int size, std::string text, bool allowNewLines);
    int textBeforeWidth(const std::string& name, int size, std::string text, float width, bool allowNewLines);
    ~DrawText();
#ifndef USE_EXTERNAL_FOLDER_RESOURCES
    const std::string VERTEX_SHADER = "res/shaders/text_v.glsl";
    const std::string FRAGMENT_SHADER = "res/shaders/text_f.glsl";
#else
    const std::string VERTEX_SHADER = "../res/shaders/text_v.glsl";
    const std::string FRAGMENT_SHADER = "../res/shaders/text_f.glsl";
#endif
    const std::string FONT_BASE_PATH = "res/fonts/";
private:

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
        bool init = false;
        std::map<GLchar, Character> characters;
    };

    std::map<std::string, std::map<int, Font>> fonts;
};

#endif //UNTITLED2_DRAW_TEXT_H
