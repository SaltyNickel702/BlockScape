#include "Logic.h"
#include "World.h"

LObject::LObject () : pos(glm::vec3(0.0f)), rot(glm::vec2(0.0f)) {
	World::LogicObjects.push_back(this); //automatically adds to logic object collection
}