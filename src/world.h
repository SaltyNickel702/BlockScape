#include <iostream>
#include <vector>
#include <map>

#include "Block.h"
#include "Chunk.h"
#include "Model.h"

using namespace std;

#ifndef WORLD_H
#define WORLD_H

namespace World { //world data
    extern map<int, map<int,Chunk>> chunks;
    const vector<float*> getChunkMesh (int cx, int cy); //two float arrays, one for vertices, one for indices
    static vector<float*> joinChunkMeshes (int x, int y, Chunk chunks[]);

    extern map<int,Block> blockTypes;

    extern vector<Model*> models;
    
    void setBlock (int x, int y, int z, int block);
    int getBlock (int x, int y, int z);
    
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
