#include "Logic.h"
#include "World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace std;

LObject::LObject () : pos(glm::vec3(0.0f)), rot(glm::vec2(0.0f)), active(false) {
	World::LogicObjects.push_back(this); //automatically adds to logic object collection
}
glm::vec3* LObject::raycast (float limit, float step, bool getLastAir) {
	glm::mat4 rotMatrix(1.0f);
	rotMatrix = glm::rotate(rotMatrix, -glm::radians(rot.x), glm::vec3(0,1,0));
	rotMatrix = glm::rotate(rotMatrix, -glm::radians(rot.y), glm::vec3(1,0,0));
	glm::vec3 forwardVec = rotMatrix * glm::vec4(0,0,1,1) * glm::vec4(1,-1,1,1);
	

	float distance = 0;
	glm::vec3 stepV = step*forwardVec;
	glm::vec3 p = pos;
	
	while(distance < limit) {
		distance+=step;
		p = p + stepV;
		int* b = World::getBlock(p.x,p.y,p.z);
		if (*b != 0) {
			if (getLastAir) p = p - stepV;
			
			return new glm::vec3{p};
		};
	}
	return nullptr;
}