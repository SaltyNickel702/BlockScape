#include "Engine.h"

using namespace Engine;

//Private namespace
namespace {
	void windowResizeCallback(GLFWwindow* window, int width, int height) { //for when the window gets resized
		glViewport(0, 0, width, height);
		Engine::width = width;
		Engine::height = height;
	}


	//Input Handeling
	glm::vec2 mouseLastPos(0);
	void processInput(GLFWwindow* window) {
		//esc key closes app (temporary)
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);


		//Adding/removing keys to keyDown | If new key is added, add to keyDownTick for one tick
		for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++) {
			if (keyDownTick[i]) keyDownTick[i] = false;
			bool isDown = (glfwGetKey(window, i) == GLFW_PRESS);
			if (isDown) {
				bool previous = keyDown[i];
				keyDown[i] = true;
				if (!previous) {
					keyDownTick[i] = true;
				}
			} else keyDown[i] = false;
		}
		//Same for Mouse
		for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i++) {
			if (mouseDownTick[i]) mouseDownTick[i] = false;
			bool isDown = (glfwGetMouseButton(window,i) == GLFW_PRESS);
			if (isDown) {
				bool previous = mouseDown[i];
				mouseDown[i] = true;
				if (!previous) {
					mouseDownTick[i] = true;
				}
			} else mouseDown[i] = false;
		}
	
		//Get Mouse Position
		double pos[2];
		glfwGetCursorPos(window,&pos[0],&pos[1]);
		Engine::cursorPos = glm::vec2(floor(pos[0]),floor(pos[1]));
		if (!Engine::cursorEnabled) {
			glfwSetCursorPos(window,0,0);
		}
	}
	//last input stuff

	bool running = true;
	float lastFrame = 0;
	float currentFrame = 0;
	void tick () {
		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
		deltaTick =  currentFrame - lastFrame;

		while (tickQueue.size() > 0) {
			tickQueue[0]();
			tickQueue.erase(tickQueue.begin());
		}

		int objs = 0;
		for (LObject *o: World::LogicObjects) {
			if ((o->active || find(o->activeStates.begin(),o->activeStates.end(), GameState::currentState) != o->activeStates.end()) && o->onTick) {
				o->onTick();
			}
			objs++;
		}
	}
}

//Exported Game namespace
namespace Engine {
	GLFWwindow* window = nullptr;
	int width, height;

	float deltaTick = 0;

	unsigned int genTextureAtlas (const vector<string>& imgNames) {
		GLuint atlasTex;
		glGenTextures(1, &atlasTex);
		glBindTexture(GL_TEXTURE_2D, atlasTex);
		
		// Allocate empty texture space
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16*imgNames.size(), 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		
		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Load and copy each texture into the atlas
		for (size_t i = 0; i < imgNames.size(); i++) {
			int width, height, channels;
			string path = "./assets/textures/" + imgNames.at(i);
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
			
			if (data) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, i * 16, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			} else {
				std::cerr << "Failed to load texture: " << path << std::endl;
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		return atlasTex;
	};

	unsigned int genTexture (string ImgName) { //make sure to set active texture before loading
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //use closest pixel color, not mixed
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		string ImgRel = "./assets/textures/" + ImgName;
		int width, height, nrChannels;
		unsigned char *data = stbi_load(ImgRel.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			int format;
			if (nrChannels == 4) {
				format = GL_RGBA;
			} else if (nrChannels == 3) {
				format = GL_RGB;
			} else if (nrChannels == 1) {
				format = GL_RED;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			cout << "Failed to Load Texture: " << ImgName << endl;
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}

	int init(int w, int h) {
		//Initialize
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Set Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Use core version of OpenGL
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //FOR MACOS
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		//Create GLFW window
		window = glfwCreateWindow(w, h, "Block Scape", NULL, NULL); //Size, title, monitor, shared recourses
		if (window == NULL) {
			cout << "Failed to create GLFW window" << endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);


		//Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			cout << "Failed to initialize GLAD" << endl;
			return -1;
		}

		//Sets GL Viewport (camera)
		glViewport(0, 0, w, h);
		Engine::width = w;
		Engine::height = h;
		glfwSetFramebufferSizeCallback(window,windowResizeCallback); //assigns resize callback function

		// #ifdef __APPLE__
		// 	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// #endif

		return 0;
	}


	//Input Handeling

	bool cursorEnabled = true;
	bool mouseDown[GLFW_MOUSE_BUTTON_LAST+1];
	bool mouseDownTick[GLFW_MOUSE_BUTTON_LAST+1];
	glm::vec2 cursorPos;
	void allowCursor (bool b) {
		cursorEnabled = b;
		if (!b) { //disable
			Engine::cursorPos = glm::vec2(0);
			glfwSetCursorPos(window,0,0);
		}
		glfwSetInputMode(window, GLFW_CURSOR, b ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		if (b) { //enable
			Engine::cursorPos = glm::vec2(Engine::width/2, Engine::height/2);
			glfwSetCursorPos(window,400,400);
		}
	}

	bool keyDown[GLFW_KEY_LAST+1];
	bool keyDownTick[GLFW_KEY_LAST+1];

	// Add global variables for menu rendering
	Shader* menuShader = nullptr;
	unsigned int menuTexture = 0;
	unsigned int menuVAO = 0, menuVBO = 0;

	// void initMenu() {
	// 	// Load the menu shaders
	// 	menuShader = new Shader("menuVert.glsl", "menuFrag.glsl");

	// 	// Load the menu texture
	// 	menuTexture = genTexture("menu.png");

	// 	// Ensure the shader uniform is set correctly
	// 	menuShader->use();
	// 	glUniform1i(glGetUniformLocation(menuShader->ID, "menuTexture"), 0); // Bind to texture unit 0

	// 	// Set up a quad for rendering the menu
	// 	float vertices[] = {
	// 		// positions   // texCoords
	// 		-1.0f,  1.0f,  0.0f, 0.0f, // Flip UV vertically
	// 		-1.0f, -1.0f,  0.0f, 1.0f,
	// 		 1.0f, -1.0f,  1.0f, 1.0f,
	// 		 1.0f,  1.0f,  1.0f, 0.0f
	// 	};
	// 	unsigned int indices[] = {
	// 		0, 1, 2,
	// 		0, 2, 3
	// 	};

	// 	glGenVertexArrays(1, &menuVAO);
	// 	glGenBuffers(1, &menuVBO);

	// 	glBindVertexArray(menuVAO);

	// 	glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
	// 	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	// 	glEnableVertexAttribArray(0);

	// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	// 	glEnableVertexAttribArray(1);

	// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// 	glBindVertexArray(0);
	// }

	// void renderMenu() {
	// 	menuShader->use();
	// 	int textureLocation = glGetUniformLocation(menuShader->ID, "menuTexture");

	// 	glBindTexture(GL_TEXTURE_2D, menuTexture);
	// 	glBindVertexArray(menuVAO);
	// 	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// 	glBindVertexArray(0);
	// }

	void loop() {
		// Enable face culling
				
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		// initMenu();

		while (!glfwWindowShouldClose(window)) {
			//RENDERING
			glClearColor(.5,.7,.8,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			processInput(window);
			tick(); //Any logic is run through tick

			glfwSwapBuffers(window); //updates screen buffer
			glfwPollEvents(); //Check for inputs
		}
		running = false;
		glfwTerminate();
	}

	vector<function<void()>> tickQueue;
}
