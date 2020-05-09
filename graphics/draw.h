//
// Created by isaac on 9/11/2019.
//

#ifndef UNTITLED_DRAW_H
#define UNTITLED_DRAW_H
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include "constants.h"
#include "draw_square.h"
#include "draw_text.h"
#include "draw_image.h"
#include "draw_model.h"
#include "draw_model_textured.h"
#include "texture_load.h"

#define RENDER_2
#ifdef RENDER_2

class Graphics;

class Graphics {
public:
    class Property {
    protected:
        Graphics& graphics;
        explicit Property(Graphics& graphics);
    };

    class Object : public Property {
        virtual void draw() = 0;
    protected:
        explicit Object(Graphics& graphics);
    };

    class Model : public Object {
        virtual const std::vector<Triangle>& getTriangles() const = 0;
        virtual glm::mat4 getModelMatrix() const = 0;
    protected:
        explicit Model(Graphics& graphics);
    };

    struct Rectangle : public Object {
        glm::mat4 setup = glm::mat4(1.0f);
        glm::mat4 matrix = glm::mat4(1.0f);
        glm::vec4 color = glm::vec4(1.0f);
        void draw() override;
    private:
        explicit Rectangle(Graphics& graphics);
        friend class Graphics;
    };

    struct Text : public Object {
        glm::mat4 setup = glm::mat4(1.0f);
        glm::mat4 matrix = glm::mat4(1.0f);
        glm::vec4 color = glm::vec4(1.0f);
        std::string text = "";
        bool allowNewLines = true;
        void draw() override;
        float getWidth(const std::string& txt);
        int getTextInWidth(const std::string& txt, float width);
        const std::string font;
        const int size;
    private:
        explicit Text(Graphics& graphics, std::string font, int size);
        friend class Graphics;
    };

    struct Image : public Object {
        glm::mat4 setup = glm::mat4(1.0f);
        glm::mat4 matrix = glm::mat4(1.0f);
        glm::vec4 color = glm::vec4(1.0f);
        std::string image;
        void draw() override;
    private:
        explicit Image(Graphics& graphics);
        friend class Graphics;
    };

    struct PlainModel : public Model {
        glm::mat4 setup = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(1.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::vec4 color = glm::vec4(1.0f);
        float diffusePotency = 0;
        float baseLightRadius = 0;
        void draw() override;
        const std::vector<Triangle>& getTriangles() const override;
        virtual glm::mat4 getModelMatrix() const;
        ~PlainModel();
    private:
        explicit PlainModel(Graphics& graphics, const std::string& modelPath);
        DrawModel::ModelRef model;
        friend class Graphics;
    };

    struct PlainTextureModel : public Model {
        glm::mat4 setup = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(1.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::vec4 color = glm::vec4(1.0f);
        float diffusePotency = 0;
        float baseLightRadius = 0;
        std::string image;
        void draw() override;
        const std::vector<Triangle>& getTriangles() const override;
        virtual glm::mat4 getModelMatrix() const;
        ~PlainTextureModel();
    private:
        explicit PlainTextureModel(Graphics& graphics, const std::string& modelPath);
        DrawTexturedModel::ModelRef model;
        friend class Graphics;
    };

    struct Light : public Property {
        glm::vec3 position;
        glm::vec3 color;
        float strength;
        ~Light();
    private:
        explicit Light(Graphics& graphics);
        friend class Graphics;
    };

    Rectangle initRectangle();
    Text initText(const std::string& font, int size);
    Image initImage();
    PlainModel initPlainModel(const std::string& modelPath);
    PlainTextureModel initPlainTextureModel(const std::string& modelPath);
    Light initLight();
    Graphics();
    ~Graphics();

    glm::vec3 ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
private:
    DrawSquare drawSquare;
    DrawText drawText;
    DrawImage drawImage;
    DrawModel drawModel;
    DrawTexturedModel drawTexturedModel;
    TextureLibrary textureLibrary;
    std::set<Light*> lights;
};
#endif

#ifdef RENDER_1

class Model;
class Graphics {
public:
    Graphics();
    ~Graphics();
    struct Context {
        Context();
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
        glm::vec4 color;
        int texture[8] = {};
        std::string text;
        int fontSize;
    } context;
    struct Library {
        Library();
        Model* simpleSquare;
        Model* textureSquare;
        Model* font;
    } library;
private:
    std::vector<Model*> models;
    std::vector<GLint> shaders;
};
class Model {
    virtual void draw(const Graphics::Context& context) = 0;
};
int createShaderProgram(const std::string& vsource, const std::string& fsource, const std::string& gsource = "");
#endif

#ifdef RENDER_0
namespace draw {

    struct TextureMode;

    void errorCheck(const std::string& err);

    void initSquare();
    void initDrawImage();

    // fontName refers to the name of the font, without preceding res/fonts/ or succeeding .ttf extension
    void initFont(const std::string& fontName, const int &size);

    void initTexture(const std::string& path, const TextureMode& textureMode);
    void bindTexture(const std::string& path);

    inline void init() {
        glewExperimental = GL_TRUE;
        GLenum glewStatus = glewInit();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0, 0, 0, 0);
        errorCheck("draw pre-init");
        initSquare();
        initDrawImage();
    }

    void destroySquare();
    void destroyAllText();
    void destroyDrawImage();

    inline void destroy() {
        errorCheck("draw end");
        destroyDrawImage();
        destroySquare();
        destroyAllText();
        errorCheck("draw destroy");
    }

    void square(const glm::mat4 &matrix, const glm::vec4 &color = glm::vec4(1.0f));
    void text(const std::string& fontName, const int &size, const std::string &text, const glm::mat4 &matrix,
                  const glm::vec4 &color = glm::vec4(1.0f));
    void image(const glm::mat4 &matrix, const glm::vec4 &color = glm::vec4(1.0f));
    float textWidth(const std::string& fontName, const int& size, const std::string& text);

    inline const std::map<std::string, GLint> GLOBAL_ATTRIBUTE_LOCATIONS = { // NOLINT(cert-err58-cpp)
            {"position", 0},
            {"texture",  1},
            {"normal",   2}
    };

    inline const std::map<std::string, std::string> RESOURCES = { // NOLINT(cert-err58-cpp)
            {"fonts", "res/fonts/"},
            {"shaders/simple_v", "res/shaders/simple_v.glsl"},
            {"shaders/simple_f", "res/shaders/simple_f.glsl"},
            {"shaders/text_v", "res/shaders/text_v.glsl"},
            {"shaders/text_f", "res/shaders/text_f.glsl"},
            {"shaders/texture_v", "res/shaders/texture_v.glsl"},
            {"shaders/texture_f", "res/shaders/texture_f.glsl"}
    };

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
    };
}

struct WindowContext {
    GLFWwindow *window = nullptr;
    int width = 0, height = 0;

    std::function<void(GLFWwindow *, int, int)> sizeChange = [](GLFWwindow *w, int a, int b) {};

    WindowContext(GLFWwindow *window, int width, int height);

    GLFWwindow *shareWindow();
};

void createGLFW();

std::unique_ptr<WindowContext> createWindow(const std::string &title, int width, int height);
GLint createShaderProgram(const std::string &vsrcFile, const std::string &fsrcFile, const std::string &gsrcFile, const std::vector<std::string> &attributes);
#endif

#include <iostream>

inline void errorCheckGl(const std::string &err) {
    GLenum e = glGetError();
    if(e) {
        std::cerr << "GL Error Code: " << e << ", " << err << std::endl;
    }
}

#endif //UNTITLED_DRAW_H
