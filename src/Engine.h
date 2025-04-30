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
#include <algorithm>
#include <ctime>
#include <fstream>
#include <filesystem>

//Project Classes
#include "Shader.h"
#include "Chunk.h"
#include "World.h"
#include "Block.h"
#include "Model.h"
#include "Logic.h"
#include "UI.h"
#include "GameState.h"


using namespace std;

//Game engine namespace, not the BlockScape Game
namespace Engine {
	extern GLFWwindow* window;
	extern int width, height;
	extern float deltaTick; //Mutliply rate/second by deltaTick to get rate/tick
	
	int init(int w, int h); //Starts OpenGL
	void loop (); //Starts the game loop
	

	extern bool cursorEnabled; //Tells if cursor is enabled | Do not directly control this
	void allowCursor(bool boolean); //lock cursor or not
	extern bool mouseDown[GLFW_MOUSE_BUTTON_LAST+1]; //Bool of whether button is down or not
	extern bool mouseDownTick[GLFW_MOUSE_BUTTON_LAST+1]; //Same as Engine::mouseDown, but only true for one tick
	extern glm::vec2 cursorPos; //cursor position when cursor is enable, else is mouse movement

	extern bool keyDown[GLFW_KEY_LAST+1]; //Bool of whether key is down or not
	extern bool keyDownTick[GLFW_KEY_LAST+1]; //Same as Engine::keyDown, but only true for one tick
	
	extern unsigned int textureAtlas;
	unsigned int genTextureAtlas (const vector<string>& imgNames);
	unsigned int genTexture (string ImgName); //generates texture on graphics card from png file and returns texture ID

	extern vector<function<void()>> tickQueue;
}

#endif
