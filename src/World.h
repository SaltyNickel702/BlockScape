#include <iostream>
#include <fstream>
#include <vector>
#include <map>


#include "Block.h"
#include "Chunk.h"
#include "Model.h"
#include "Logic.h"
#include "Shader.h"

using namespace std;

#ifndef WORLD_H
#define WORLD_H

namespace World { //world data
    void loadNew (int seed); //Load data into chunk files
    void loadFromSave (string saveFolder);
    void saveGame (string saveFolder);

    extern map<int, map<int,Chunk>> chunks;

    extern map<int,Block> blockTypes;
    extern map<string,Shader*> shaders;
    extern map<string, unsigned int*> textures;

    extern vector<Model*> models;
    extern vector<LObject*> LogicObjects;
    
    Chunk* getChunk (int x, int y);
    int* getBlock (int x, int y, int z);
    void setBlock (int x, int y, int z, int block);
    
    namespace Settings {
        extern float FOV;
        extern float renderDistance; //radius
    }

    extern LObject Camera;
    extern LObject Player;

    extern int seed;
}

#endif