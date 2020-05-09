//
// Created by isaac on 5/9/2020.
//

#include <glm/gtc/matrix_transform.hpp>
#include "Chatbox.h"

Chatbox::Chatbox(Graphics::Text text, int log_length, int space, int pixel_width, glm::mat4 origin)
: text(text), log_length(log_length), space(space), pixel_width(pixel_width), origin(origin) {}
void Chatbox::println(const std::string& s) {
    std::string current = s;
    text.allowNewLines = true;
    do {
        int pos = text.getTextInWidth(current, pixel_width);
        std::string cut = current.substr(0, pos);
        std::string remain = current.substr(pos, current.size());

        // add back words that are cut between lines
        if(pos < current.size() &&
        !(remain.front() == '\n' || cut.back() == '\n' || remain.front() == ' ' || cut.back() == ' ')) {
            for(int i = cut.size() - 1; i >= 0; --i) {
                if(cut[i] == ' ') {
                    pos = i + 1;
                    break;
                }
            }
            cut = current.substr(0, pos);
            remain = current.substr(pos, current.size());
        }

        log.push_back(cut);
        if (log.back().size() == 0 && remain.front() != '\n') {
            log.pop_back();
            return;
        } else {
            if(log.size() > MAX_LOG) {
                log.erase(log.begin());
            }
        }

        current = remain;
    } while(current.size() > 0);
}
void Chatbox::draw(double delta, const glm::mat4& setup) {
    if(!typing) {
        inactivity += delta;
    }

    if(inactivity < 5) {
        text.setup = setup;
        text.color = glm::vec4(1, 1, 1, std::min<float>(1, 5 - inactivity));

        text.matrix = origin;
        if (typed.size() > 0 || typing) {
            text.text = typed;
            if (typing) {
                bar_counter += delta;
                int bc_round = (int) (bar_counter * 2);
                if (bc_round < 1) {
                    text.text.push_back('|');
                }
                if (bc_round >= 2) {
                    bar_counter -= bc_round / 2.0f;
                }
            }
            text.allowNewLines = false;
            text.draw();
        }

        text.allowNewLines = true;
        int min = std::max<int>(0, log.size() - log_length);
        for (int i = log.size() - 1; i >= min; --i) {
            text.matrix = glm::translate(text.matrix, glm::vec3(0, space, 0));
            text.text = log[i];
            text.draw();
        }
    }
}
void Chatbox::enable() {
    typing = true;
    bar_counter = 0;
    inactivity = 0;
}
void Chatbox::disable() {
    typing = false;
    inactivity = 0;
}