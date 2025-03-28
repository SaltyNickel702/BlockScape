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

//Project Classes
#include "Shader.h"
#include "chunk.h"
#include "world.h"
#include "Block.h"
#include "Model.h"


using namespace std;


namespace Game {
	extern GLFWwindow* window;
	unsigned int genTexture (string ImgName);
	
	int init(int w, int h);
	bool keyDown(int GLFWkey); //is key held Down
	void addKeydownCallback(int GLFWkey, const function<void()>& func); //triggers passed in function when key is pressed
	
}

#endif
