#define STB_IMAGE_IMPLEMENTATION 
#include "Engine.h" //includes all needed includes

vector<string> blockTextures {"GrassSide.png","GrassTop.png","Dirt.png","Stone.png","LogTop.png","LogSide.png","Leaves.png","Sand.png","OakPlank.png"};


void DefineBlocks() {
    Block airBlock("Air", 0);
    airBlock.tranparent = true;
    World::blockTypes[0] = airBlock;

    Block grassBlock("Grass", 1);
    grassBlock.tranparent = false;
    grassBlock.textureSide = 0;
    grassBlock.textureTop = 1;
    grassBlock.textureBottom = 2;
    World::blockTypes[1] = grassBlock;

    Block dirtBlock("Dirt", 2);
    dirtBlock.tranparent = false;
    dirtBlock.textureSide = 2;
    dirtBlock.textureTop = 2;
    dirtBlock.textureBottom = 2;
    World::blockTypes[2] = dirtBlock;
    
    Block stoneBlock("Stone", 3);
    stoneBlock.tranparent = false;
    stoneBlock.textureSide = 3;
    stoneBlock.textureTop = 3;
    stoneBlock.textureBottom = 3;
    World::blockTypes[3] = stoneBlock;

    Block woodBlock("Wood", 4);
    woodBlock.tranparent = false;
    woodBlock.textureSide = 5;
    woodBlock.textureTop = 4;
    woodBlock.textureBottom = 4;
    World::blockTypes[4] = woodBlock;

    Block leavesBlock("Leaves", 5);
    leavesBlock.tranparent = true;
    leavesBlock.textureSide = 6;
    leavesBlock.textureTop = 6;
    leavesBlock.textureBottom = 6;
    World::blockTypes[5] = leavesBlock;

    Block sandBlock("Sand", 6);
    sandBlock.tranparent = false;
    sandBlock.textureSide = 7;
    sandBlock.textureTop = 7;
    sandBlock.textureBottom = 7;
    World::blockTypes[6] = sandBlock;

    Block woodPlank("Wooden Plank", 7);
    woodPlank.tranparent = false;
    woodPlank.textureSide = 8;
    woodPlank.textureTop = 8;
    woodPlank.textureBottom = 8;
    World::blockTypes[7] = woodPlank;
}

void DefineLogicObjects() {
    UI::menuTick.onTick = [&](){
        using namespace UI;
        UI::hoveringOverButton = false;


        for (Menu* m : menus) {
            if (!(m->visible || find(m->activeStates.begin(),m->activeStates.end(),GameState::currentState) != m->activeStates.end())) continue;
            for (Image* i : m->images) {
                i->imgMesh->draw();
            }
            for (Button* b : m->buttons) {
                b->images.at(b->currentImg)->imgMesh->draw();
            }
            for (Text* t : m->texts) {
                t->draw();
            }
            
            if (!Engine::cursorEnabled) continue;
            for (Button* b : m->buttons) {
                glm::vec2 c = Engine::cursorPos;
                glm::vec2 p = b->images.at(b->currentImg)->pos;
                glm::vec2 d = b->images.at(b->currentImg)->dim;
                if (c.x >= p.x - .5*d.x && c.x <= p.x + .5*d.x && c.y >= p.y - .5*d.y && c.y <= p.y + .5*d.y) {
                    UI::hoveringOverButton = true;
                    if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_LEFT]) {
                        Engine::tickQueue.push_back(b->onClick);
                    }
                    if (!b->hovering) {
                        b->hovering = true;
                        Engine::tickQueue.push_back(b->onHover);
                    }
                } else if (b->hovering) {
                    b->hovering = false;
                    Engine::tickQueue.push_back(b->onLeave);
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
        using namespace World::PlayerData;

        LObject* p = &World::Player; //shortcut for not having to write World::Player each time; to access player attributes, use p->attribute, not p.attribute

        //Temp Camera Rotation
        float rotSpeed = 90*Engine::deltaTick;
        if (Engine::cursorEnabled) {
            if (Engine::keyDown[GLFW_KEY_LEFT]) p->rot.x-= rotSpeed;
            if (Engine::keyDown[GLFW_KEY_RIGHT]) p->rot.x+= rotSpeed;
            if (Engine::keyDown[GLFW_KEY_UP]) p->rot.y-= rotSpeed;
            if (Engine::keyDown[GLFW_KEY_DOWN]) p->rot.y+= rotSpeed;
        } else {
            p->rot+= 0.1f*Engine::cursorPos; //floating point coefficient determines sensitivity
        }
        // cout << Engine::cursorPos.x << " " << Engine::cursorPos.y << " -- ";
        
        if (p->rot.y > 90) p->rot.y = 90;
        if (p->rot.y < -90) p->rot.y = -90;
        if (p->rot.x < 0 || p->rot.x > 360) p->rot.x = fmod(p->rot.x + 360,360);
        
        //Rotation Matrices
        glm::mat4 rotMatrix(1.0f);
        rotMatrix = glm::rotate(rotMatrix, -glm::radians(World::Camera.rot.x), glm::vec3(0,1,0));
        // rotMatrix = glm::rotate(rotMatrix, -glm::radians(World::Camera.rot.y), glm::vec3(1,0,0)); //Uncomment to make movement relative camera y instead of only x rotation
        glm::vec3 forwardVec = rotMatrix * glm::vec4(0,0,1,1) * glm::vec4(1,-1,1,1);
        glm::vec3 sideVec = rotMatrix * glm::vec4(1,0,0,1) * glm::vec4(1,-1,1,1);
        glm::vec3 upVec(0,1,0);
        
        // mode change. VERY TEMPORARY
        if (Engine::keyDownTick[GLFW_KEY_1]) {
            CurrentMode = GameMode::SPECTATOR;
            cout << "Spectator Mode" << endl;
        } else if (Engine::keyDownTick[GLFW_KEY_2]) {
            CurrentMode = GameMode::CREATIVE;
            Flying = true;
            cout << "Creative Mode" << endl;
        } else if (Engine::keyDownTick[GLFW_KEY_3]) {
            CurrentMode = GameMode::SURVIVAL;
            Flying = false;
            cout << "Survival Mode" << endl;
        }

        //Movement
        float speed = 10.0*Engine::deltaTick; //multiply speed per second by deltaTick to get speed in last frame
        if (CurrentMode == GameMode::SPECTATOR) {           // spectator movement
            if (Engine::keyDown[GLFW_KEY_LEFT_CONTROL]) speed*=5;//2.5
            if (Engine::keyDown[GLFW_KEY_W]) p->pos = p->pos + speed * forwardVec;
            if (Engine::keyDown[GLFW_KEY_S]) p->pos = p->pos - speed * forwardVec;
            if (Engine::keyDown[GLFW_KEY_A]) p->pos = p->pos + speed * sideVec;
            if (Engine::keyDown[GLFW_KEY_D]) p->pos = p->pos - speed * sideVec;
            if (Engine::keyDown[GLFW_KEY_LEFT_SHIFT]) p->pos = p->pos - speed * upVec;
            if (Engine::keyDown[GLFW_KEY_SPACE]) p->pos = p->pos + speed * upVec;
        } else {
            if (Flying == true) {           //Creative flying
                glm::vec3 previousPos = p->pos; // position to check against
                if (Engine::keyDown[GLFW_KEY_LEFT_CONTROL]) speed*=5;//2.5
                if (Engine::keyDown[GLFW_KEY_W]) p->pos = p->pos + speed * forwardVec;
                if (Engine::keyDown[GLFW_KEY_S]) p->pos = p->pos - speed * forwardVec;
                if (Engine::keyDown[GLFW_KEY_A]) p->pos = p->pos + speed * sideVec;
                if (Engine::keyDown[GLFW_KEY_D]) p->pos = p->pos - speed * sideVec;
                if (Engine::keyDown[GLFW_KEY_LEFT_SHIFT]) p->pos = p->pos - speed * upVec;
                if (Engine::keyDown[GLFW_KEY_SPACE]) p->pos = p->pos + speed * upVec;
                // x axis
                if (isColliding(p->pos.x, previousPos.y, previousPos.z)) {
                    p->pos.x = previousPos.x;
                }
                // y axos
                if (isColliding(p->pos.x, p->pos.y, previousPos.z)) {
                    p->pos.y = previousPos.y;
                }
                // z axis
                if (isColliding(p->pos.x, p->pos.y, p->pos.z)) {
                    p->pos.z = previousPos.z;
                }
            } else {        //Survival + Creative walking
                glm::vec3 previousPos = p->pos;
                if (!onGround) {
                    velocity.y -= 35.0f * Engine::deltaTick; // gravity. val to change for diff grav phys
                }
                if (velocity.y < -22.0f) velocity.y = -22.0f; // settign a max fall speed. val to change for diff grav phys
                
                glm::vec3 walkDir = glm::vec3(0.0f);

                // change these to an inWater bool 
                // if we ever get water that isint just below a certain level
                float pspeed;
                if (p->pos.y <= 29){
                    pspeed = 2.3f; // below water. val to change for diff grav phys
                } else if (p->pos.y > 29){
                    pspeed = 5.5f; // above water . val to change for diff grav phys
                }
                if (Engine::keyDown[GLFW_KEY_LEFT_CONTROL]) pspeed *= 1.5f; // "running" multiplier (its a brisk walk at best)
                if (Engine::keyDown[GLFW_KEY_W]) walkDir += forwardVec;
                if (Engine::keyDown[GLFW_KEY_S]) walkDir -= forwardVec;
                if (Engine::keyDown[GLFW_KEY_A]) walkDir += sideVec;
                if (Engine::keyDown[GLFW_KEY_D]) walkDir -= sideVec;

                if (glm::length(walkDir) > 0) {
                    walkDir = glm::normalize(walkDir) * pspeed * Engine::deltaTick;
                }
                p->pos.x += walkDir.x;
                p->pos.z += walkDir.z;
                if (isColliding(p->pos.x, previousPos.y, previousPos.z)) {
                    p->pos.x = previousPos.x;
                }
                if (isColliding(p->pos.x, previousPos.y, p->pos.z)) {
                    p->pos.z = previousPos.z;
                }
                p->pos.y += velocity.y * Engine::deltaTick;
                onGround = false;
                if (isColliding(p->pos.x, p->pos.y, p->pos.z)) {
                    if (velocity.y < 0) { // Moving down
                        p->pos.y = floor(previousPos.y) + 0.6f; // snap to ground
                        onGround = true;
                        velocity.y = 0;
                    } else { // moving up
                        p->pos.y = previousPos.y;
                        velocity.y = 0;
                    }
                }
                // jumping
                if (onGround && Engine::keyDown[GLFW_KEY_SPACE]) {
                    velocity.y = 10.0f; // jump speed. val to change for diff grav phys
                    onGround = false;
                }
            }
        }

        //Block Placing
        if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_LEFT]) {
            glm::vec3* bcPtr = World::Camera.raycast(6,.1);
            if (bcPtr != nullptr) {
                glm::vec3 bc = *bcPtr;
                // delete bcPtr;
                World::setBlock(bc.x,bc.y,bc.z,0);

            }
        }
        if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_RIGHT]) {
            World::setBlock(p->pos.x,p->pos.y,p->pos.z,5);
        }
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
    
    Image* start = new Image(*World::textures["startButton"],Engine::width/2,Engine::height/2,46*10,16*10);
    Button* startBtn = new Button(start);
    startBtn->onClick = [&]() {
        World::loadFromSave("newWorld");
        GameState::currentState = GameState::State::PLAYING;
    };
    mainMenu->buttons.push_back(startBtn);
    mainMenu->activeStates = vector<GameState::State> {GameState::State::MENU};

    World::menus["mainMenu"] = mainMenu;
}

void AddToggleKeybinds () { //things like menu opening
    LObject* global = new LObject();
    global->active = true;

    LObject* mainMenu = new LObject();
    mainMenu->activeStates = vector<GameState::State>{GameState::State::MENU};

    LObject* pauseMenu = new LObject();
    pauseMenu->activeStates = vector<GameState::State>{GameState::State::PAUSE};

    LObject* running = new LObject();
    running->onTick = [&](){
        if (Engine::keyDownTick[GLFW_KEY_ENTER]) {
            Engine::allowCursor(!Engine::cursorEnabled);
        }
        if (Engine::keyDownTick[GLFW_KEY_C]) {
            cout << World::Camera.pos.x << " " << World::Camera.pos.y << " " << World::Camera.pos.z << endl;
        }
        if (Engine::keyDownTick[GLFW_KEY_P]) {
            World::saveGame("newWorld");
        }
    };
    running->activeStates = vector<GameState::State>{GameState::State::PLAYING};
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

        glUniform3f(glGetUniformLocation(worldShader->ID,"cameraPos"),World::Camera.pos.x,World::Camera.pos.y,World::Camera.pos.z);
        glUniform2f(glGetUniformLocation(worldShader->ID,"cameraRot"),World::Camera.rot.x,World::Camera.rot.y);
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
    // World::menus["mainMenu"]->visible = true;

    // World::loadNew(495804);
    // World::loadNew(54123453);
    World::loadFromSave("newWorld");
    // World::loadNew(time(0));

    Engine::loop();
}