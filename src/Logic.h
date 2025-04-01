#include <functional>
#include <glm/glm.hpp>

#ifndef LOGIC_H
#define LOGIC_H

class LObject {
    public:
        LObject ();
        glm::vec3 pos;
        glm::vec2 rot;
        std::function<void()> onTick = [](){};
};
#endif