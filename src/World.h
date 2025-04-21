#include <iostream>
#include <fstream>
#include <vector>
#include <map>


#include "Block.h"
#include "Chunk.h"
#include "Model.h"
#include "Logic.h"
#include "Shader.h"
#include "UI.h"

using namespace std;

#ifndef WORLD_H
#define WORLD_H

//Everything related to BlockScape
namespace World {
    void loadNew (int seed); //Load data into chunk files
    void loadFromSave (string saveFolder);
    void saveGame (string saveFolder);

    extern map<int, map<int,Chunk>> chunks;

    //These maps have no functional purpose, they are only here so we can reference them
    extern map<int,Block> blockTypes;
    extern map<string,Shader*> shaders;
    extern map<string, unsigned int*> textures;
    extern map<string, UI::Menu*> menus;

    extern vector<Model*> models;
    extern vector<LObject*> LogicObjects;
    
    Chunk* getChunk (float x, float z); //world coords
    Chunk* getChunkByCC (int cx, int cy); //chunk coords;
    int* getBlock (int x, int y, int z);
    void setBlock (int x, int y, int z, int block);
    
    namespace Settings {
        extern float FOV;
        extern int renderDistance; //radius
    }

    extern LObject Camera;
    extern LObject Player;
    namespace PlayerData {
        enum class GameMode {
            SPECTATOR,
            CREATIVE,
            SURVIVAL
        };
        extern GameMode CurrentMode;
        extern bool Flying;
    }

    extern int seed;	
}

#endif