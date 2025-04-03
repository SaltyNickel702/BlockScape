#define STB_IMAGE_IMPLEMENTATION 
#include "Game.h" //includes all needed includes


void DefineBlocks() {
    Block airBlock("Air", 0);
    World::blockTypes[0] = airBlock;

    Block grassBlock("Grass", 1);
    grassBlock.textureTop = 1;
    grassBlock.textureSide = 1;
    grassBlock.textureBottom = 1;
    World::blockTypes[1] = grassBlock;

    Block dirtBlock("Dirt", 2);
    World::blockTypes[2] = dirtBlock;
    
    Block stoneBlock("Stone", 3);
    World::blockTypes[3] = stoneBlock;

    Block woodBlock("Wood", 4);
    World::blockTypes[4] = woodBlock;

    Block leavesBlock("Leaves", 5);
    World::blockTypes[5] = leavesBlock;
}

void DefineLogicObjects() {
    World::Player.onTick = [&](){
        LObject* p = &World::Player; //shortcut for not having to write World::Player each time; to access player attributes, use p->attribute, not p.attribute

    };

    World::Camera.pos = glm::vec3(0,34,0);
    World::Camera.rot = glm::vec2(0,0);
    World::Camera.onTick = [&](){
        LObject* c = &World::Camera; //shortcut for not having to write World::Camera each time

        //Temp Camera Rotation
        float rotSpeed = 90*Game::deltaTick;
        if (Game::cursorEnabled) {
            if (Game::keyDown(GLFW_KEY_LEFT)) c->rot.x-= rotSpeed;
            if (Game::keyDown(GLFW_KEY_RIGHT)) c->rot.x+= rotSpeed;
            if (Game::keyDown(GLFW_KEY_UP)) c->rot.y-= rotSpeed;
            if (Game::keyDown(GLFW_KEY_DOWN)) c->rot.y+= rotSpeed;
        } else {
            c->rot+= 0.1f*Game::cursorPos; //floating point coefficient determines sensitivity
        }
        // cout << Game::cursorPos.x << " " << Game::cursorPos.y << " -- ";
        
        if (c->rot.y > 90) c->rot.y = 90;
        if (c->rot.y < -90) c->rot.y = -90;
        if (c->rot.x < 0 || c->rot.x > 360) c->rot.x = fmod(c->rot.x + 360,360);

        //Temp Rotation matrix
        glm::mat4 rotMatrix(1.0f);
        rotMatrix = glm::rotate(rotMatrix, -glm::radians(World::Camera.rot.x), glm::vec3(0,1,0));
        // rotMatrix = glm::rotate(rotMatrix, -glm::radians(World::Camera.rot.y), glm::vec3(1,0,0)); //Uncomment to make movement relative camera y instead of only x rotation
        glm::vec3 forwardVec = rotMatrix * glm::vec4(0,0,1,1) * glm::vec4(1,-1,1,1);
        glm::vec3 sideVec = rotMatrix * glm::vec4(1,0,0,1) * glm::vec4(1,-1,1,1);
        glm::vec3 upVec(0,1,0);

        //Temp Camera Movement
        float speed = 6.0*Game::deltaTick; //multiply speed per second by deltaTick to get speed in last frame
        if (Game::keyDown(GLFW_KEY_LEFT_CONTROL)) speed*=2.5;
        if (Game::keyDown(GLFW_KEY_W)) c->pos = c->pos + speed*forwardVec;
        if (Game::keyDown(GLFW_KEY_S)) c->pos = c->pos - speed*forwardVec;
        if (Game::keyDown(GLFW_KEY_A)) c->pos = c->pos + speed*sideVec;
        if (Game::keyDown(GLFW_KEY_D)) c->pos = c->pos - speed*sideVec;
        if (Game::keyDown(GLFW_KEY_LEFT_SHIFT)) c->pos = c->pos - speed*upVec;
        if (Game::keyDown(GLFW_KEY_SPACE)) c->pos = c->pos + speed*upVec;


        // cout << c->rot.x << " " << c->rot.y << " -- ";
        // cout << c->pos.x << " " << c->pos.y << " " << c->pos.z << endl;
    };

}

void AddToggleKeybinds () { //things like menu opening
    Game::addKeydownCallback(GLFW_KEY_ENTER,[&](){Game::allowCursor(!Game::cursorEnabled);});
}

int main () {
    DefineBlocks();
    DefineLogicObjects();
    AddToggleKeybinds(); //for other keybinds that are checked each frame, use logic objects + bool Game::keyDown(GLFW_KEY_)

    Game::init(1200,800);
}