#include <functional>
#include <vector>
#include <glm/glm.hpp>

#include "GameState.h"

#ifndef LOGIC_H
#define LOGIC_H

//Object that runs on every game tick
//Can represent an entity in the world
class LObject {
    public:
        LObject ();
        glm::vec3 pos;
        glm::vec2 rot; //left right ; up down  |  (0 is left, 360 is toward right) ; (-90 is up, 90 is down)
        std::function<void()> onTick;
        
        std::vector<GameState::State> activeStates;
        bool active; // Will run even if currentState isn't listed under activeStates
};
#endif