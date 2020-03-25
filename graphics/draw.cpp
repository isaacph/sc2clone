//
// Created by isaac on 9/23/2019.
//

//
// Created by isaac on 9/17/2019.
//

#include "draw.h"
#include <iostream>

Graphics::Graphics() {

}

Graphics::~Graphics() {

}

Graphics::Object::Object(Graphics& graphics) : graphics(graphics) {}


Graphics::Rectangle::Rectangle(Graphics& graphics) : Object(graphics) {}
void Graphics::Rectangle::draw() {
    graphics.drawSquare.draw(matrix, color);
}
Graphics::Rectangle Graphics::initRectangle() {
    return Graphics::Rectangle(*this);
}

Graphics::Text::Text(Graphics& graphics, std::string font, int size) : Object(graphics), font(std::move(font)), size(size) {}
void Graphics::Text::draw() {
    graphics.drawText.drawFont(matrix, color, font, size, text);
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
    graphics.drawImage.draw(matrix, color, 0);
}
Graphics::Image Graphics::initImage() {
    return Graphics::Image(*this);
}