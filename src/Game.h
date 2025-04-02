#ifndef GAME_H
#define GAME_H

//External Libraries
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <functional>
#include <vector>
#include <FastNoise/FastNoiseLite.h>
#include <stbi/stb_image.h>
#include <cmath>

//Project Classes
#include "Shader.h"
#include "Chunk.h"
#include "World.h"
#include "Block.h"
#include "Model.h"
#include "Logic.h"


using namespace std;


namespace Game {
	extern GLFWwindow* window;
	unsigned int genTexture (string ImgName);
	extern float deltaTick;
	
	int init(int w, int h);
	

	extern bool cursorEnabled;
	extern glm::vec2 cursorPos; //cursor position when cursor is enable, else is mouse movement

	bool keyDown(int GLFWkey); //is key held Down
	void addKeydownCallback(int GLFWkey, const function<void()>& func); //triggers passed in function when key is pressed
	void allowCursor(bool boolean); //lock cursor or not
	

	unsigned int genTexture (string ImgName); //generates texture on graphics card from png file and returns texture ID
}

#endif
