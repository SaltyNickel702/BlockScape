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
	extern int width, height;
	extern float deltaTick;
	
	int init(int w, int h);
	void loop ();
	

	extern bool cursorEnabled;
	extern glm::vec2 cursorPos; //cursor position when cursor is enable, else is mouse movement

	bool keyDown(int GLFWkey); //is key held Down
	void addKeydownCallback(int GLFWkey, const function<void()>& func); //triggers passed in function when key is pressed
	void allowCursor(bool boolean); //lock cursor or not
	
	extern unsigned int textureAtlas;
	unsigned int genTextureAtlas (const vector<string>& imgNames);
	unsigned int genTexture (string ImgName); //generates texture on graphics card from png file and returns texture ID

	extern vector<function<void()>> tickQueue;
}

#endif
