#define STB_IMAGE_IMPLEMENTATION 
#include "Game.h" //includes all needed includes


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
    World::Player.onTick = [&](){
        LObject* p = &World::Player; //shortcut for not having to write World::Player each time; to access player attributes, use p->attribute, not p.attribute

        //Temp Camera Rotation
        float rotSpeed = 90*Game::deltaTick;
        if (Game::cursorEnabled) {
            if (Game::keyDown(GLFW_KEY_LEFT)) p->rot.x-= rotSpeed;
            if (Game::keyDown(GLFW_KEY_RIGHT)) p->rot.x+= rotSpeed;
            if (Game::keyDown(GLFW_KEY_UP)) p->rot.y-= rotSpeed;
            if (Game::keyDown(GLFW_KEY_DOWN)) p->rot.y+= rotSpeed;
        } else {
            p->rot+= 0.1f*Game::cursorPos; //floating point coefficient determines sensitivity
        }
        // cout << Game::cursorPos.x << " " << Game::cursorPos.y << " -- ";
        
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
        float speed = 10.0*Game::deltaTick; //multiply speed per second by deltaTick to get speed in last frame
        if (Game::keyDown(GLFW_KEY_LEFT_CONTROL)) speed*=5;//2.5
        if (Game::keyDown(GLFW_KEY_W)) p->pos = p->pos + speed*forwardVec;
        if (Game::keyDown(GLFW_KEY_S)) p->pos = p->pos - speed*forwardVec;
        if (Game::keyDown(GLFW_KEY_A)) p->pos = p->pos + speed*sideVec;
        if (Game::keyDown(GLFW_KEY_D)) p->pos = p->pos - speed*sideVec;
        if (Game::keyDown(GLFW_KEY_LEFT_SHIFT)) p->pos = p->pos - speed*upVec;
        if (Game::keyDown(GLFW_KEY_SPACE)) p->pos = p->pos + speed*upVec;
    };

    World::Camera.pos = glm::vec3(0,34,0);
    World::Camera.rot = glm::vec2(0,0);
    World::Camera.onTick = [&](){
        LObject* c = &World::Camera; //shortcut for not having to write World::Camera each time

        c->pos = World::Player.pos + glm::vec3(0,2,0);
        c->rot = World::Player.rot;


        // cout << c->rot.x << " " << c->rot.y << " -- ";
        // cout << c->pos.x << " " << c->pos.y << " " << c->pos.z << endl;
    };

}

void AddToggleKeybinds () { //things like menu opening
    Game::addKeydownCallback(GLFW_KEY_ENTER,[&](){Game::allowCursor(!Game::cursorEnabled);});
    Game::addKeydownCallback(GLFW_KEY_C,[&](){
        cout << World::Camera.pos.x << " " << World::Camera.pos.y << " " << World::Camera.pos.z << endl;
    });
    Game::addKeydownCallback(GLFW_KEY_M,[&](){
        if(Game::currentState == Game::GameState::MENU) {
            Game::currentState = Game::GameState::PLAYING;
            Game::allowCursor(!Game::cursorEnabled);
        }else if(Game::currentState == Game::GameState::PLAYING) {
            Game::currentState = Game::GameState::MENU;
            Game::allowCursor(!Game::cursorEnabled);
        }
    });
}

int main () {
    //Load Game First
    //None OpenGL things first
    DefineBlocks();
    DefineLogicObjects();
    AddToggleKeybinds(); //for other keybinds that are checked each frame, use logic objects + bool Game::keyDown(GLFW_KEY_)

    //Initialize OpenGL
    Game::init(1200,800);
    cout << "creating GLFW" << endl;


    //Compile Assets
    cout << "Generating Textures" << endl;
    vector<string> textures {"GrassSide.png","GrassTop.png","Dirt.png","Stone.png","LogTop.png","LogSide.png"};
    unsigned int atlas = Game::genTextureAtlas(textures);
    World::textures["atlas"] = &atlas;


    cout << "Generating Shaders" << endl;
    Shader shaderProgram("worldVert.glsl","worldFrag.glsl");
    shaderProgram.uniforms = [&](glm::vec3 pos, glm::vec2 rot) {
        float timeValue = glfwGetTime();
        glUniform1f(glGetUniformLocation(shaderProgram.ID,"time"),timeValue);

        glUniform1i(glGetUniformLocation(shaderProgram.ID,"renderDistance"),World::Settings::renderDistance*16);

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
        project = glm::perspective(glm::radians(World::Settings::FOV), (float)Game::width/Game::height, 0.1f, 16.0f*World::Settings::renderDistance*2);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID,"model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID,"view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID,"projection"), 1, GL_FALSE, glm::value_ptr(project));

        glUniform1i(glGetUniformLocation(shaderProgram.ID,"totalTextures"),textures.size());

    };
    World::shaders["world"] = &shaderProgram;


    //Add menu stuff here
    World::loadNew(495804);

    Game::loop();
}