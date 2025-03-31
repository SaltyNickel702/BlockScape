#define STB_IMAGE_IMPLEMENTATION 
#include "Game.h" //includes all needed includes


void DefineBlocks() {
    Block airBlock("Air", 0);
    World::blockTypes[0] = airBlock;

    Block grassBlock("Grass", 1);
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

int main () {
    Game::init(800,800);

}