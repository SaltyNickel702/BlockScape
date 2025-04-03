#include <iostream>
#include <vector>
#include <map>


#include "Block.h"
#include "Chunk.h"
#include "Model.h"
#include "Logic.h"

using namespace std;

#ifndef WORLD_H
#define WORLD_H

namespace World { //world data
    extern map<int, map<int,Chunk>> chunks;

    extern map<int,Block> blockTypes;

    extern vector<Model*> models;

    extern vector<LObject*> LogicObjects;
    
    Chunk* getChunk (int x, int y);
    int* getBlock (int x, int y, int z);
    void setBlock (int x, int y, int z, int block);
    
    namespace CameraConfig {
        extern float FOV;
    }
    extern LObject Camera;
    extern LObject Player;
}

#endif
