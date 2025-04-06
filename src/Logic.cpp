#include "Logic.h"
#include "World.h"
#include <iostream>
using namespace std;

LObject::LObject () : pos(glm::vec3(0.0f)), rot(glm::vec2(0.0f)), debugVal(World::LogicObjects.size()) {
	World::LogicObjects.push_back(this); //automatically adds to logic object collection
}