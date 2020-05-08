//
// Created by isaac on 9/23/2019.
//

//
// Created by isaac on 9/17/2019.
//

#include "draw.h"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>

Graphics::Graphics() {
}

Graphics::~Graphics() {

}

Graphics::Property::Property(Graphics& graphics) : graphics(graphics) {}
Graphics::Object::Object(Graphics& graphics) : Property(graphics) {}
Graphics::Model::Model(Graphics& graphics) : Object(graphics) {}

Graphics::Rectangle::Rectangle(Graphics& graphics) : Object(graphics) {}
void Graphics::Rectangle::draw() {
    graphics.drawSquare.draw(setup * matrix, color);
}
Graphics::Rectangle Graphics::initRectangle() {
    return Graphics::Rectangle(*this);
}

Graphics::Text::Text(Graphics& graphics, std::string font, int size) : Object(graphics), font(std::move(font)), size(size) {}
void Graphics::Text::draw() {
    graphics.drawText.drawFont(setup * matrix, color, font, size, text);
}
Graphics::Text Graphics::initText(const std::string& font, int size) {
    drawText.initFont(font, size);
    return Graphics::Text(*this, font, size);
}
float Graphics::Text::width() {
    return graphics.drawText.textWidth(font, size, text);
}

Graphics::Image::Image(Graphics& graphics) : Object(graphics) {}
void Graphics::Image::draw() {
    graphics.textureLibrary.bindTexture(image, {});
    graphics.drawImage.draw(setup * matrix, color, 0);
}
Graphics::Image Graphics::initImage() {
    return Graphics::Image(*this);
}

Graphics::PlainModel::PlainModel(Graphics &graphics, const std::string& s) : Model(graphics),
model(graphics.drawModel.initModel(s)) {}
void Graphics::PlainModel::draw() {
    std::vector<glm::vec3> lightp;
    std::vector<glm::vec3> lightc;
    std::vector<float> lighta;
    auto it = graphics.lights.begin();
    float radius = baseLightRadius * std::max(scale.x, std::max(scale.y, scale.z)) * 2;
    for(size_t i = 0; i < graphics.lights.size() && lightp.size() < SHADER_NUM_LIGHTS; ++i) {
        auto& light = **it++;
        glm::vec3 diff = light.position - position;
        float dist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
        if(light.strength > 0 && (light.strength >= 100000 || dist * light.strength < radius)) {
            lightp.push_back(light.position);
            lightc.push_back(light.color);
            lighta.push_back(light.strength);
        }
    }
    glm::mat4 model_matrix = getModelMatrix();
    glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    DrawModel::DrawParameters params = {setup, model_matrix, normal, color, graphics.ambientColor, lightp, lightc, lighta};
    graphics.drawModel.draw(model, params);
    errorCheckGl("Plain model draw");
}
Graphics::PlainModel::~PlainModel() {
    graphics.drawModel.clearModel(model);
}
Graphics::PlainModel Graphics::initPlainModel(const std::string &modelPath) {
    return Graphics::PlainModel(*this, modelPath);
}
const std::vector<Triangle>& Graphics::PlainModel::getTriangles() const {
    return model.triangles;
}
glm::mat4 Graphics::PlainModel::getModelMatrix() const {
    return glm::scale(glm::translate(glm::mat4(1.0f), position) * rotation, scale);
}

Graphics::PlainTextureModel::PlainTextureModel(Graphics &graphics, const std::string& s) : Model(graphics),
        model(graphics.drawTexturedModel.initModel(s)) {}
void Graphics::PlainTextureModel::draw() {
    graphics.textureLibrary.bindTexture(image, {});
    std::vector<glm::vec3> lightp;
    std::vector<glm::vec3> lightc;
    std::vector<float> lighta;
    auto it = graphics.lights.begin();
    float radius = baseLightRadius * std::max(scale.x, std::max(scale.y, scale.z)) * 2;
    for(size_t i = 0; i < graphics.lights.size() && lightp.size() < SHADER_NUM_LIGHTS; ++i) {
        auto& light = **it++;
        glm::vec3 diff = light.position - position;
        float dist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
        if(light.strength > 0 && (light.strength >= 100000 || dist * light.strength < radius)) {
            lightp.push_back(light.position);
            lightc.push_back(light.color);
            lighta.push_back(light.strength);
        }
    }
    glm::mat4 model_matrix = getModelMatrix();
    glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    DrawTexturedModel::DrawParameters params = {setup, model_matrix, normal, color, graphics.ambientColor, lightp, lightc, lighta};
    graphics.drawTexturedModel.draw(model, params);
    errorCheckGl("Plain model draw");
}
Graphics::PlainTextureModel::~PlainTextureModel() {
    graphics.drawTexturedModel.clearModel(model);
}
Graphics::PlainTextureModel Graphics::initPlainTextureModel(const std::string &modelPath) {
    return Graphics::PlainTextureModel(*this, modelPath);
}
const std::vector<Triangle>& Graphics::PlainTextureModel::getTriangles() const {
    return model.triangles;
}
glm::mat4 Graphics::PlainTextureModel::getModelMatrix() const {
    return glm::scale(glm::translate(glm::mat4(1.0f), position) * rotation, scale);
}

Graphics::Light Graphics::initLight() {
    return Graphics::Light(*this);
}
Graphics::Light::Light(Graphics& graphics) : Property(graphics) {
    graphics.lights.insert(this);
}
Graphics::Light::~Light() {
    graphics.lights.erase(this);
}