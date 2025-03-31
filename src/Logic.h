#include <functional>
#include <glm/glm.hpp>

#ifndef LOGIC_H
#define LOGIC_H

class LObject {
    public:
        // LObject () {};
        glm::vec3 pos;
        std::function<void()> onTick = [](){};
};
#endif