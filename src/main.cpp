#define STB_IMAGE_IMPLEMENTATION 
#include "Engine.h" //includes all needed includes

vector<string> blockTextures {"GrassSide.png","GrassTop.png","Dirt.png","Stone.png","LogTop.png","LogSide.png","Leaves.png","Sand.png","OakPlank.png","GlassBlock.png"};


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

    Block glassBlock("Glass", 8);
    glassBlock.tranparent = true;
    glassBlock.textureSide = 9;
    glassBlock.textureTop = 9;
    glassBlock.textureBottom = 9;
    World::blockTypes[8] = glassBlock;
}

void DefineLogicObjects() {
    UI::menuTick.onTick = [&](){
        using namespace UI;
        UI::hoveringOverButton = false;


        for (Menu* m : menus) {
            if (!(m->visible || find(m->activeStates.begin(),m->activeStates.end(),GameState::currentState) != m->activeStates.end())) continue;
            if (m->onTick) m->onTick();


            for (Element* e : m->elements) {
                Textbox* tb = dynamic_cast<Textbox*>(e);

                if (tb) { //editting textboxes
                    Text* t = tb->text;
                    if (t->editing) {
                        bool update = false;
                        if (Engine::keyDownTick[GLFW_KEY_BACKSPACE] && t->text.size() > 0) {
                            t->text.pop_back();

                            update = true;
                        }
                        if (Engine::keyDownTick[GLFW_KEY_SPACE] && t->text.size() < tb->maxCharacterLength) {
                            t->text.push_back(' ');
                            update = true;
                        } 
                        for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_GRAVE_ACCENT; i++) {
                            if (Engine::keyDownTick[i]) {
                                if (find(t->f->chars.begin(),t->f->chars.end(),tolower((char)i)) != t->f->chars.end()) {
                                    if (t->text.size() < tb->maxCharacterLength) {
                                        t->text.push_back((char)i);
                                        update = true;
                                    }   
                                }
                            }
                        }
                        if (update) t->genMesh();


                        //Cursor Position Thing
                        t->elapsedTime += Engine::deltaTick;
                        if (t->elapsedTime >= 1/t->cursorTickRate) {
                            t->elapsedTime = 0;
                            t->cursorVisible ^= true; //flips value
                            t->genMesh();
                        }
                    } else if (t->cursorVisible) {
                        t->cursorVisible = false;
                        t->genMesh();
                    };
                }

                e->draw();

                if (!Engine::cursorEnabled || !e->clickable) continue;
                bool clicked = false;
                if (e->mouseOver()) {
                    hoveringOverButton = true;
                    if (!e->hovering) {
                        e->hovering = true;
                        e->onHover();
                    }
                    if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_LEFT]) {
                        clicked = true;
                        e->onClick();
                        if (tb) {
                            tb->text->editing = true;
                            tb->text->cursorVisible = true;
                            tb->text->genMesh();
                        }
                    }
                } else if (e->hovering) {
                    e->hovering = false;
                    e->onLeave();
                }

                if (tb && Engine::mouseDownTick[GLFW_MOUSE_BUTTON_LEFT] && !clicked) { //clicked something else
                    tb->text->editing = false;
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
                    velocity.y -= 35.0f * Engine::deltaTick; // gravity. val to change for different physics.
                }
                if (velocity.y < -22.0f) velocity.y = -22.0f; // max fall speed. val to change for different physics.
                
                glm::vec3 walkDir = glm::vec3(0.0f);
                glm::vec3 acceleration = glm::vec3(0.0f);
                float baseAccel;

                if (p->pos.y <= 29){
                    baseAccel = 20.0f; // in water. val to change for different physics.
                } else {
                    baseAccel = 70.0f; // on ground. val to change for different physics.
                }

                // Apply acceleration based on input
                if (Engine::keyDown[GLFW_KEY_W] && onGround) {acceleration += forwardVec;}
                else if (Engine::keyDown[GLFW_KEY_W] && !onGround) {acceleration += forwardVec * 0.5f;}
                if (Engine::keyDown[GLFW_KEY_S] && onGround) {acceleration -= forwardVec;}
                else if (Engine::keyDown[GLFW_KEY_S] && !onGround) {acceleration -= forwardVec * 0.5f;}
                if (Engine::keyDown[GLFW_KEY_A] && onGround) {acceleration += sideVec;}
                else if (Engine::keyDown[GLFW_KEY_A] && !onGround) {acceleration += sideVec * 0.5f;}
                if (Engine::keyDown[GLFW_KEY_D] && onGround) {acceleration -= sideVec;}
                else if (Engine::keyDown[GLFW_KEY_D] && !onGround) {acceleration -= sideVec * 0.5f;}

                if (glm::length(acceleration) > 0) {
                    acceleration = glm::normalize(acceleration) * baseAccel;
                    if (Engine::keyDown[GLFW_KEY_LEFT_CONTROL] && onGround) acceleration *= 1.5f;
                }

                // Apply friction when not accelerating or in water
                float friction;
                if (p->pos.y <= 29) {
                    friction = 3.0f;  // In water. val to change for different physics.
                } else {
                    if (onGround) {
                        friction = 20.0f;  // On ground. val to change for different physics.
                    } else {
                        friction = 1.0f;  // In air. val to change for different physics.
                    }
                }
                glm::vec3 horizontalVel = glm::vec3(velocity.x, 0.0f, velocity.z);
                float speedSq = glm::dot(horizontalVel, horizontalVel);
                
                if (speedSq > 0) {
                    glm::vec3 frictionForce = -horizontalVel * friction * Engine::deltaTick;
                    velocity += frictionForce;
                }

                // Apply acceleration to velocity
                velocity += acceleration * Engine::deltaTick;

                // Clamp horizontal speed
                float maxSpeed;
                if (p->pos.y <= 29){
                    maxSpeed = 2.3f; //val to change for different physics.
                } else {
                    maxSpeed = 5.5f; //val to change for different physics.
                }
                if (Engine::keyDown[GLFW_KEY_LEFT_CONTROL]) maxSpeed *= 1.5f;
                
                horizontalVel = glm::vec3(velocity.x, 0.0f, velocity.z);
                if (glm::length(horizontalVel) > maxSpeed) {
                    horizontalVel = glm::normalize(horizontalVel) * maxSpeed;
                    velocity.x = horizontalVel.x;
                    velocity.z = horizontalVel.z;
                }

                // X axis
                p->pos.x += velocity.x * Engine::deltaTick;
                if (isColliding(p->pos.x, previousPos.y, previousPos.z)) {
                    p->pos.x = previousPos.x;
                    velocity.x = 0; // Stop horizontal momentum on collision
                }

                // Z axis
                p->pos.z += velocity.z * Engine::deltaTick;
                if (isColliding(p->pos.x, previousPos.y, p->pos.z)) {
                    p->pos.z = previousPos.z;
                    velocity.z = 0; // Stop horizontal momentum on collision
                }

                // Y axis
                p->pos.y += velocity.y * Engine::deltaTick;
                onGround = false;
                if (isColliding(p->pos.x, p->pos.y, p->pos.z)) {
                    if (velocity.y < 0) { // Moving down
                        p->pos.y = floor(previousPos.y) + 0.6f; // snap to ground
                        onGround = true;
                    } else { // moving up
                        p->pos.y = previousPos.y;
                    }
                    velocity.y = 0;
                }

                // jumping
                if (onGround && Engine::keyDown[GLFW_KEY_SPACE]) {
                    velocity.y = 10.3f; // jump speed. val to change for different physics.
                    onGround = false;
                }
            }
        }

        //Block Placing
        if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_LEFT]) {
            glm::vec3* bcPtr = World::Camera.raycast(6,.02,false);
            if (bcPtr != nullptr) {
                glm::vec3 bc = *bcPtr;
                delete bcPtr;
                World::setBlock(bc.x,bc.y,bc.z,0);
            }
        } 
        if (Engine::mouseDownTick[GLFW_MOUSE_BUTTON_RIGHT]) {
            glm::vec3* bcPtr = World::Camera.raycast(6,.02,true);
            if (bcPtr != nullptr) {
                glm::vec3 bc = *bcPtr;
                delete bcPtr;
                World::setBlock(bc.x,bc.y,bc.z,5);
            }
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

    Image* menuBackground = new Image(*World::textures["menuBackground"], Engine::width/2, Engine::height/2, 800, 600);
    menuBackground->center();

    #pragma region "Main Menu"
    Menu* mainMenu = new Menu();

    Text* titleText = new Text(World::fonts["main"],Engine::width/2, Engine::height/3);
    titleText->setHeight(200);
    titleText->setText("BlockScape");
    titleText->center();
    mainMenu->elements.push_back(titleText);
    
    Image* startBtn = new Image(*World::textures["startButton"],Engine::width/2,Engine::height/3*2,46*10,16*10);
    startBtn->clickable = true;
    startBtn->onClick = [&]() {
        GameState::currentState = GameState::State::LOAD_SELECT;
    };
    startBtn->center();
    mainMenu->elements.push_back(startBtn);

    mainMenu->activeStates = vector<GameState::State> {GameState::State::MENU};
    World::menus["mainMenu"] = mainMenu;
    #pragma endregion

    #pragma region "Load Method Selection"
    Menu* loadSelect = new Menu();

    loadSelect->elements.push_back(menuBackground);

    Text* newWorldBtn = new Text(World::fonts["main"], Engine::width/2, Engine::height/2 - 50);
    newWorldBtn->setText("New World");
    newWorldBtn->setHeight(50);
    newWorldBtn->center();
    newWorldBtn->clickable = true;
    newWorldBtn->onClick = [&]() {
        GameState::currentState = GameState::State::LOAD_NEW;
    };
    loadSelect->elements.push_back(newWorldBtn);

    Text* loadWorldBtn = new Text(World::fonts["main"], Engine::width/2, Engine::height/2 + 50);
    loadWorldBtn->setText("Load World");
    loadWorldBtn->setHeight(50);
    loadWorldBtn->center();
    loadWorldBtn->clickable = true;
    loadWorldBtn->onClick = [&]() {
        // GameState::currentState = GameState::State::LOAD_FROM_SAVE;
        World::loadFromSave("newWorld");
        GameState::currentState = GameState::State::PLAYING;
    };
    loadSelect->elements.push_back(loadWorldBtn);

    loadSelect->activeStates = vector<GameState::State> {GameState::State::LOAD_SELECT};
    World::menus["loadSelect"] = loadSelect;
    #pragma endregion


    #pragma region "GUI"
    Menu* GUI = new Menu();

    Image* Crosshair = new Image(*World::textures["Crosshair"],Engine::width/2, Engine::height/2, 16, 16);
    Crosshair->center();
    GUI->elements.push_back(Crosshair);

    GUI->activeStates = vector<GameState::State> {GameState::State::PLAYING, GameState::State::PAUSE};
    GUI->visible = true;
    World::menus["GUI"] = GUI;
    #pragma endregion
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

    Shader* textShader = new Shader("textVert.glsl", "textFrag.glsl");
    World::shaders["text"] = textShader;
    textShader->uniforms = [&](glm::vec3 pos, glm::vec2 rot) {
        glDisable(GL_DEPTH_TEST);
        Shader* textShader = World::shaders["text"];

        glm::mat4 model(1);
        model = glm::translate(model,pos);
        glUniformMatrix4fv(glGetUniformLocation(textShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniform4f(glGetUniformLocation(textShader->ID, "screen"), Engine::width, Engine::height,1,1);

        glUniform1f(glGetUniformLocation(textShader->ID,"time"), glfwGetTime());
    };
}

void genTextures () {
    cout << "Generating Textures" << endl;

    unsigned int* atlas = new unsigned int(Engine::genTextureAtlas(blockTextures));
    World::textures["atlas"] = atlas;
    
    unsigned int* menuBackground = new unsigned int(Engine::genTexture("MenuBackground.png"));
    World::textures["menuBackground"] = menuBackground;

    unsigned int* startButton = new unsigned int(Engine::genTexture("StartButton.png"));
    World::textures["startButton"] = startButton;
    
    unsigned int* textbox = new unsigned int(Engine::genTexture("Textbox.png"));
    World::textures["Textbox"] = textbox;

    unsigned int* crosshair = new unsigned int(Engine::genTexture("Crosshair1.png"));
    World::textures["Crosshair"] = crosshair;



    UI::Font* mainFont = new UI::Font("Font.png", 16, 30);
    mainFont->chars = "abcdefghijklmnopqrstuvwxyz";
    World::fonts["main"] = mainFont;

}

int main () {
    //Load Game First
    //Non- OpenGL things first
    DefineBlocks();
    DefineLogicObjects();
    AddToggleKeybinds();

    //Initialize OpenGL
    cout << "Initializing GLFW" << endl;
    Engine::init(1200,800);


    genTextures();
    genShaders();


    cout << "Defining Menus" << endl;
    defineMenus();
    // World::menus["mainMenu"]->visible = true;

    // World::loadNew(495804);
    // World::loadNew(54123453);
    // World::loadNew(time(0));

    if (GameState::currentState == GameState::State::PLAYING) {
        // World::loadNew(495804);
        // World::loadNew(54123453);
        World::loadFromSave("newWorld");
        // World::loadNew(time(0));
    }
    
    Engine::loop();
}