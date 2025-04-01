#include <functional>
#include <glm/glm.hpp>

#ifndef LOGIC_H
#define LOGIC_H

class LObject {
    public:
        LObject () : pos(glm::vec3(0.0f)), rot(glm::vec2(0.0f)) {}
        glm::vec3 pos;
        glm::vec2 rot;
        std::function<void()> onTick = [](){};
};
#endif