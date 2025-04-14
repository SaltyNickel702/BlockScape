#define STB_IMAGE_IMPLEMENTATION 
#include "Engine.h" //includes all needed includes

vector<string> blockTextures {"GrassSide.png","GrassTop.png","Dirt.png","Stone.png","LogTop.png","LogSide.png"};


void DefineBlocks() {
    Block airBlock("Air", 0);
    World::blockTypes[0] = airBlock;

    Block grassBlock("Grass", 1);
    grassBlock.textureSide = 0;
    grassBlock.textureTop = 1;
    grassBlock.textureBottom = 2;
    World::blockTypes[1] = grassBlock;

    Block dirtBlock("Dirt", 2);
    dirtBlock.textureSide = 2;
    dirtBlock.textureTop = 2;
    dirtBlock.textureBottom = 2;
    World::blockTypes[2] = dirtBlock;
    
    Block stoneBlock("Stone", 3);
    stoneBlock.textureSide = 3;
    stoneBlock.textureTop = 3;
    stoneBlock.textureBottom = 3;
    World::blockTypes[3] = stoneBlock;

    Block woodBlock("Wood", 4);
    woodBlock.textureSide = 5;
    woodBlock.textureTop = 4;
    woodBlock.textureBottom = 4;
    World::blockTypes[4] = woodBlock;

    Block leavesBlock("Leaves", 5);
    World::blockTypes[5] = leavesBlock;
}

void DefineLogicObjects() {
    UI::menuTick.onTick = [&](){
        using namespace UI;
        UI::hoveringOverButton = false;


        for (Menu* m : menus) {
            if (!m->visible) continue;
            for (Image* i : m->images) {
                i->imgMesh->draw();
            }
            for (Button* b : m->buttons) {
                b->images.at(b->currentImg)->imgMesh->draw();
            }
            
            if (!Engine::cursorEnabled) continue;
            for (Button* b : m->buttons) {
                glm::vec2 c = Engine::cursorPos;
                glm::vec2 p = b->images.at(b->currentImg)->pos;
                glm::vec2 d = b->images.at(b->currentImg)->dim;
                if (c.x >= p.x - .5*d.x && c.x <= p.x + .5*d.x && c.y >= p.y - .5*d.y && c.y <= p.y + .5*d.y) {
                    UI::hoveringOverButton = true;
                    if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_LEFT]) {
                        b->onClick();
                    }
                    if (!b->hovering) {
                        b->hovering = true;
                        b->onHover();
                    }
                } else if (b->hovering) {
                    b->hovering = false;
                    b->onLeave();
                }
            }
        }
        if (UI::hoveringOverButton) {
            glfwSetCursor(Engine::window, glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR));
        } else {
            glfwSetCursor(Engine::window, nullptr);
        }
    };
    UI::menuTick.active = true; //always active

    World::Player.onTick = [&](){
        LObject* p = &World::Player; //shortcut for not having to write World::Player each time; to access player attributes, use p->attribute, not p.attribute

        //Temp Camera Rotation
        float rotSpeed = 90*Engine::deltaTick;
        if (Engine::cursorEnabled) {
            if (Engine::keyDown(GLFW_KEY_LEFT)) p->rot.x-= rotSpeed;
            if (Engine::keyDown(GLFW_KEY_RIGHT)) p->rot.x+= rotSpeed;
            if (Engine::keyDown(GLFW_KEY_UP)) p->rot.y-= rotSpeed;
            if (Engine::keyDown(GLFW_KEY_DOWN)) p->rot.y+= rotSpeed;
        } else {
            p->rot+= 0.1f*Engine::cursorPos; //floating point coefficient determines sensitivity
        }
        // cout << Engine::cursorPos.x << " " << Engine::cursorPos.y << " -- ";
        
        if (p->rot.y > 90) p->rot.y = 90;
        if (p->rot.y < -90) p->rot.y = -90;
        if (p->rot.x < 0 || p->rot.x > 360) p->rot.x = fmod(p->rot.x + 360,360);

        //Temp Rotation matrix
        glm::mat4 rotMatrix(1.0f);
        rotMatrix = glm::rotate(rotMatrix, -glm::radians(World::Camera.rot.x), glm::vec3(0,1,0));
        // rotMatrix = glm::rotate(rotMatrix, -glm::radians(World::Camera.rot.y), glm::vec3(1,0,0)); //Uncomment to make movement relative camera y instead of only x rotation
        glm::vec3 forwardVec = rotMatrix * glm::vec4(0,0,1,1) * glm::vec4(1,-1,1,1);
        glm::vec3 sideVec = rotMatrix * glm::vec4(1,0,0,1) * glm::vec4(1,-1,1,1);
        glm::vec3 upVec(0,1,0);

        //Temp Camera Movement
        float speed = 10.0*Engine::deltaTick; //multiply speed per second by deltaTick to get speed in last frame
        if (Engine::keyDown(GLFW_KEY_LEFT_CONTROL)) speed*=5;//2.5
        if (Engine::keyDown(GLFW_KEY_W)) p->pos = p->pos + speed*forwardVec;
        if (Engine::keyDown(GLFW_KEY_S)) p->pos = p->pos - speed*forwardVec;
        if (Engine::keyDown(GLFW_KEY_A)) p->pos = p->pos + speed*sideVec;
        if (Engine::keyDown(GLFW_KEY_D)) p->pos = p->pos - speed*sideVec;
        if (Engine::keyDown(GLFW_KEY_LEFT_SHIFT)) p->pos = p->pos - speed*upVec;
        if (Engine::keyDown(GLFW_KEY_SPACE)) p->pos = p->pos + speed*upVec;
    };
    World::Player.activeStates = vector<GameState::State> {GameState::State::PLAYING};

    World::Camera.pos = glm::vec3(0,34,0);
    World::Camera.rot = glm::vec2(0,0);
    World::Camera.onTick = [&](){
        LObject* c = &World::Camera; //shortcut for not having to write World::Camera each time

        c->pos = World::Player.pos + glm::vec3(0,2,0);
        c->rot = World::Player.rot;


        // cout << c->rot.x << " " << c->rot.y << " -- ";
        // cout << c->pos.x << " " << c->pos.y << " " << c->pos.z << endl;
    };
    World::Camera.activeStates = vector<GameState::State> {GameState::State::PLAYING};


}

void defineMenus () {
    using namespace UI;

    
    Menu* mainMenu = new Menu(); //Temporary

    Image* back = new Image(*World::textures["mainMenu"],Engine::width/2,Engine::height/2,Engine::width,Engine::height);
    mainMenu->images.push_back(back);
    
    Image* start = new Image(*World::textures["startButton"],Engine::width/2,Engine::height/2,46*10,16*10);
    Button* startBtn = new Button(start);
    startBtn->onClick = [&]() {
        World::menus["mainMenu"]->visible = false;
        World::loadNew(495804);
        GameState::currentState = GameState::State::PLAYING;
    };
    mainMenu->buttons.push_back(startBtn);

    World::menus["mainMenu"] = mainMenu;
}

void AddToggleKeybinds () { //things like menu opening
    Engine::addKeydownCallback(GLFW_KEY_ENTER,[&](){Engine::allowCursor(!Engine::cursorEnabled);});
    Engine::addKeydownCallback(GLFW_KEY_C,[&](){
        cout << World::Camera.pos.x << " " << World::Camera.pos.y << " " << World::Camera.pos.z << endl;
    });
}

void genShaders () {
    cout << "Generating Shaders" << endl;

    Shader* worldShader = new Shader("worldVert.glsl","worldFrag.glsl");
    World::shaders["world"] = worldShader;
    worldShader->uniforms = [&](glm::vec3 pos, glm::vec2 rot) {
        glEnable(GL_DEPTH_TEST);
        Shader* worldShader = World::shaders["world"];
        
        float timeValue = glfwGetTime();
        glUniform1f(glGetUniformLocation(worldShader->ID,"time"),timeValue);

        glUniform1i(glGetUniformLocation(worldShader->ID,"renderDistance"),World::Settings::renderDistance*16);

        //Matrices
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        // model = glm::rotate(model, glm::radians(rot.x),glm::vec3(0,1,0));
        // model = glm::rotate(model, glm::radians(rot.y)),glm::vec3(1,0,0);

        glm::mat4 view(1.0f);
        view = glm::rotate(view, glm::radians(World::Camera.rot.y), glm::vec3(1,0,0));
        view = glm::rotate(view, glm::radians(World::Camera.rot.x+180), glm::vec3(0,1,0));
        view = glm::translate(view, World::Camera.pos*glm::vec3(-1));

        glm::mat4 project;
        project = glm::perspective(glm::radians(World::Settings::FOV), (float)Engine::width/Engine::height, 0.1f, 16.0f*World::Settings::renderDistance*2);

        glUniformMatrix4fv(glGetUniformLocation(worldShader->ID,"model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(worldShader->ID,"view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(worldShader->ID,"projection"), 1, GL_FALSE, glm::value_ptr(project));

        glUniform1i(glGetUniformLocation(worldShader->ID,"totalTextures"),blockTextures.size());

    };

    Shader* menuShader = new Shader("menuVert.glsl", "menuFrag.glsl");
    World::shaders["menu"] = menuShader;
    menuShader->uniforms = [&](glm::vec3 pos, glm::vec2 rot) {
        glDisable(GL_DEPTH_TEST);
        Shader* menuShader = World::shaders["menu"];

        glm::mat4 model(1);
        model = glm::translate(model,pos);
        glUniformMatrix4fv(glGetUniformLocation(menuShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniform4f(glGetUniformLocation(menuShader->ID, "screen"), Engine::width, Engine::height,1,1);

        glUniform1f(glGetUniformLocation(menuShader->ID,"time"),glfwGetTime());
    };
}

void genTextures () {
    cout << "Generating Textures" << endl;

    unsigned int* atlas = new unsigned int(Engine::genTextureAtlas(blockTextures));
    World::textures["atlas"] = atlas;

    unsigned int* menuTemp = new unsigned int(Engine::genTexture("menu.png"));
    World::textures["mainMenu"] = menuTemp;

    unsigned int* startButton = new unsigned int(Engine::genTexture("StartButton.png"));
    World::textures["startButton"] = startButton;
}

int main () {
    //Load Game First
    //None OpenGL things first
    DefineBlocks();
    DefineLogicObjects();
    AddToggleKeybinds(); //for other keybinds that are checked each frame, use logic objects + bool Game::keyDown(GLFW_KEY_)

    //Initialize OpenGL
    cout << "Initializing GLFW" << endl;
    Engine::init(1200,800);


    genTextures();
    genShaders();

    defineMenus();
    World::menus["mainMenu"]->visible = true;

    Engine::loop();
}