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
    const Model getChunkMesh (int cx, int cy); //two float arrays, one for vertices, one for indices
    static Model joinChunkMeshes (Chunk chunks[]);

    extern map<int,Block> blockTypes;

    extern vector<Model*> models;

    extern vector<LObject*> LogicObjects;
    
    Chunk* getChunk (int x, int y);
    int* getBlock (int x, int y, int z);
    void setBlock (int x, int y, int z, int block);
    
    namespace Camera {
        extern float pos[3];
        extern float rot[2]; //pitch, yaw
        extern float FOV;
    }
    namespace Player {
        extern float pos[3];
        extern float rot[2]; //pitch, yaw
        extern int inventory[10];

        int* getRayTrace (float distance);
    }
}

#endif
