#include <functional>
#include <glm/glm.hpp>

#ifndef LOGIC_H
#define LOGIC_H

class LObject {
    public:
        LObject ();
        glm::vec3 pos;
        glm::vec2 rot; //left right ; up down  |  (0 is left, 360 is toward right) ; (-90 is up, 90 is down)
        std::function<void()> onTick = [](){};
};
#endif