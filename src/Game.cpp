#include "Game.h"

using namespace Game;

//Private namespace
namespace {
	void windowResizeCallback(GLFWwindow* window, int width, int height) { //for when the window gets resized
		glViewport(0, 0, width, height);
		Game::width = width;
		Game::height = height;
	}

	//Input Handeling
	bool keysDown[GLFW_KEY_LAST-GLFW_KEY_SPACE];
	vector<function<void()>> functionCalls[GLFW_KEY_LAST-GLFW_KEY_SPACE];
	void processInput(GLFWwindow* window) {
		//esc key closes app (temporary)
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);


		//Adding/removing keys to keysDown | If new key is added, call callback functions
		for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++) {
			bool isDown = keyDown(i);
			if (isDown) {
				bool previous = keysDown[i-GLFW_KEY_SPACE]; //
				if (!previous) {
					vector<function<void()>> funcs = functionCalls[i-GLFW_KEY_SPACE];
					for (const function<void()> func : funcs) {
						func();
					}
				}
				keysDown[i-GLFW_KEY_SPACE] = true;
			} else keysDown[i-GLFW_KEY_SPACE] = false;
		}
	}
	//last input stuff
	glm::vec2 mouseCapturePos(0);
	void mouseMoveCallback (GLFWwindow* window, double xpos, double ypos) {
		glm::vec2 fin = glm::vec2(xpos,ypos) + (Game::cursorEnabled ? glm::vec2(0.0f) : mouseCapturePos);
		mouseCapturePos = fin;
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

			while (tickQueue.size() > 0) {
				tickQueue[0]();
				tickQueue.erase(tickQueue.begin());
			}

			Game::cursorPos = mouseCapturePos;
			if (!Game::cursorEnabled) mouseCapturePos = glm::vec2(0);


			for (LObject* o: World::LogicObjects) {
				o->onTick();
			}
		}
	}
}

//Exported Game namespace
namespace Game {
	GLFWwindow* window = nullptr;
	int width, height;

	float deltaTick = 0;

	unsigned int textureAtlas;
	void genTextureAtlas (string* imgNames) {};

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

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
		Game::width = w;
		Game::height = h;
		glfwSetFramebufferSizeCallback(window,windowResizeCallback); //assigns resize callback function
		glfwSetCursorPosCallback(window, mouseMoveCallback);


		// Create Model
		vector<float> vertices {
			-0.5f,-0.5f,-0.5f,	-1.0f,-1.0f,-1.0f,	0.0f,1.0f,		//bottom left
			-0.5f,0.5f,-0.5f, 	-1.0f, 1.0f, -1.0f,	0.0f,0.0f,		//Top Left
			0.5f,-0.5f,-0.5f,	1.0f, -1.0f, -1.0f,	1.0f,1.0f,		//Bottom Right
			0.5f,0.5f,-0.5f,	1.0f, 1.0f, -1.0f,	1.0f,0.0f,		//Top Right

			-0.5f,-0.5f,0.5f,	-1.0f,-1.0f,1.0f,	1.0f,1.0f,		//bottom left
			-0.5f,0.5f,0.5f, 	-1.0f, 1.0f, 1.0f,	1.0f,0.0f,		//Top Left
			0.5f,-0.5f,0.5f,	1.0f, -1.0f, 1.0f,	0.0f,1.0f,		//Bottom Right
			0.5f,0.5f,0.5f,		1.0f, 1.0f, 1.0f,	0.0f,0.0f		//Top Right
		};
		vector<unsigned int> attr {
			3,3,2
		};
		vector<unsigned int> indices {
			0, 2, 1,
			1, 2, 3,

			4, 5, 6,
			5, 6, 7,

			0, 4, 5,
			0, 1, 5,

			2, 6, 7,
			2, 3, 7,

			1, 3, 7,
			1, 5, 7,

			0, 2, 6,
			0, 4, 6
		};
		// Model m1(vertices, indices, attr);
		Chunk c = Chunk::genChunk(0,0);
		Model m1 = c.genMesh();
		cout << c.blocks[0][31][1] << endl;

		
		Shader shaderProgram("worldVert.glsl","worldFrag.glsl");
		shaderProgram.uniforms = [&]() {
			float timeValue = glfwGetTime();
			glUniform1f(glGetUniformLocation(shaderProgram.ID,"time"),timeValue);


			//Matrices
			glm::mat4 model(1.0f);
			// model = glm::rotate(model, glm::radians(m1.rot.x),glm::vec3(0,1,0));
			// model = glm::rotate(model, glm::radians(m1.rot.y),glm::vec3(1,0,0));

			glm::mat4 view(1.0f);
			view = glm::rotate(view, glm::radians(World::Camera.rot.y), glm::vec3(1,0,0));
			view = glm::rotate(view, glm::radians(World::Camera.rot.x+180), glm::vec3(0,1,0));
			view = glm::translate(view, World::Camera.pos*glm::vec3(-1));

			glm::mat4 project;
			project = glm::perspective(glm::radians(World::CameraConfig::FOV), (float)w/h, 0.1f, 100.0f);

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
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		
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
		functionCalls[key-GLFW_KEY_SPACE].push_back(func);
	}
	void allowCursor (bool b) {
		cursorEnabled = b;
		if (!b) {
			Game::cursorPos = glm::vec2(0.0f);
			mouseCapturePos = glm::vec2(0.0f);
			glfwSetCursorPos(window,0,0);
		}
		glfwSetInputMode(window, GLFW_CURSOR, b ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		if (b) {
			Game::cursorPos = glm::vec2(Game::width/2, Game::height/2);
			mouseCapturePos = glm::vec2(Game::width/2, Game::height/2);
			glfwSetCursorPos(window,400,400);
		}
	}

	vector<function<void()>> tickQueue;
}
