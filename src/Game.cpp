#include "Game.h"

using namespace Game;

//Private namespace
namespace {
	void windowResizeCallback(GLFWwindow* window, int width, int height) { //for when the window gets resized
		glViewport(0, 0, width, height);
	}

	//Input Handeling
	bool keysDown[GLFW_KEY_LAST-GLFW_KEY_SPACE];
	vector<function<void()>> functionCalls[GLFW_KEY_LAST-GLFW_KEY_SPACE];
	void processInput(GLFWwindow* window) {
		//esc key closes app (temporary)
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);


		//Adding/removing keys to keysDown | If new key is added, call callback functions
		for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; i++) {
			bool isDown = keyDown(i);
			if (isDown) {
				bool previous = keysDown[i-33]; //
				if (!previous) {
					vector<function<void()>> funcs = functionCalls[i-33];
					for (const function<void()> func : funcs) {
						func();
					}
				}
				keysDown[i-33] = true;
			} else keysDown[i-33] = false;
		}
	}
	//last input stuff
	void mouseMoveCallback (GLFWwindow* window, double xpos, double ypos) {
		glm::vec2 fin = glm::vec2(xpos,ypos) + (Game::cursorEnabled ? glm::vec2(0.0f) : Game::cursorPos);
		Game::cursorPos = fin;
		
		if (!Game::cursorEnabled) glfwSetCursorPos(window,0,0);
	}

	bool running = true;
	float lastFrame = 0;
	float currentFrame = 0;
	void tick () {
		while (running) {
			lastFrame = currentFrame;
			currentFrame = glfwGetTime();
			deltaTick =  currentFrame - lastFrame;


			for (LObject* o: World::LogicObjects) {
				o->onTick();
			}

			//reset Mouse Position if disabled;
			if (!Game::cursorEnabled) Game::cursorPos/=2;
		}
	}
}

//Exported Game namespace
namespace Game {
	GLFWwindow* window = nullptr;

	float deltaTick = 0;

	unsigned int genTexture (string ImgName) { //make sure to set active texture before loading
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		string ImgRel = "./assets/textures/" + ImgName;
		int width, height, nrChannels;
		unsigned char *data = stbi_load(ImgRel.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			cout << "Failed to Load Texture" << endl;
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
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //FOR MACOS


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
		glfwSetFramebufferSizeCallback(window,windowResizeCallback); //assigns resize callback function
		glfwSetCursorPosCallback(window, mouseMoveCallback);


		//Create Model
		vector<float> vertices {
			-0.5f,-0.5f,0.0f,	0.0f,1.0f,		//bottom left
			-0.5f,0.5f,0.0f, 	0.0f,0.0f,		//Top Left
			0.5f,-0.5f,0.0f,	1.0f,1.0f,		//Bottom Right
			0.5f,0.5f,0.0f,		1.0f,0.0f		//Top Right
		};
		vector<unsigned int> attr {
			3,2
		};
		vector<unsigned int> indices {
			0, 2, 1,
			1, 2, 3
		};
		Model m1(vertices, indices, attr);

		
		Shader shaderProgram("basicVert.glsl","basicFrag.glsl");
		shaderProgram.uniforms = [&]() {
			float timeValue = glfwGetTime();
			glUniform1f(glGetUniformLocation(shaderProgram.ID,"time"),timeValue);


			//Matrices
			glm::mat4 model(1.0f);
			model = glm::rotate(model,glm::radians(-55.0f), glm::vec3(1.0f,0.0f,0.0f));

			glm::mat4 view(1.0f);
			view = glm::translate(view, glm::vec3(0.0f,0.0f,-3.0f));

			glm::mat4 project;
			project = glm::perspective(glm::radians(45.0f), (float)w/h, 0.1f, 100.0f);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID,"model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID,"view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID,"projection"), 1, GL_FALSE, glm::value_ptr(project));

		};

		glActiveTexture(GL_TEXTURE0);
		unsigned int texture1 = Game::genTexture("GrassSide.png");
		
		m1.textures.push_back(texture1);
		m1.shader = &shaderProgram;

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Render loop
		thread tickFunc(tick);
		glEnable(GL_DEPTH_TEST);
		while(!glfwWindowShouldClose(window)) {
			processInput(window);

			//RENDERING
			glClearColor(.1f,.5f,.4f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m1.draw();
	
			glfwSwapBuffers(window); //updates screen buffer
			glfwPollEvents(); //Check for inputs
		}
		running = false;
		tickFunc.detach();
		glfwTerminate();

		return 0;
	}


	bool cursorEnabled = true;
	glm::vec2 cursorPos;

	bool keyDown (int key) {
		return glfwGetKey(window, key) == GLFW_PRESS;
	}
	void addKeydownCallback(int key, const function<void()>& func) {
		functionCalls[key-33].push_back(func);
	}
	void allowCursor (bool b) {
		cursorEnabled = b;
		glfwSetInputMode(window, GLFW_CURSOR, b ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}