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
    void loadNew (string name, int seed); //Load data into chunk files
    int loadFromSave (string saveFolder); // returns 0 if success, -1 if failure
    void saveGame (string saveFolder);
    void QuitWorld ();

    extern map<int, map<int,Chunk>> chunks;

    //These maps have no functional purpose, they are only here so we can reference them
    extern map<int,Block> blockTypes;
    extern map<string,Shader*> shaders;
    extern map<string, unsigned int*> textures;
    extern map<string, UI::Menu*> menus;
    extern map<string, UI::Font*> fonts;
    // extern map<string, Sound*> sounds;

    extern vector<Model*> models;
    extern vector<LObject*> LogicObjects;
    
    Chunk* getChunk (float x, float z); //world coords
    Chunk* getChunkByCC (int cx, int cy); //chunk coords;
    int* getBlock (float x, float y, float z);
    void setBlock (float x, float y, float z, int block);
    void placeBlock (float x, float y, float z, int block); //same as setblock, use when player is placing things
    
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
        bool isColliding(float x, float y, float z);
        extern glm::vec3 Pdim;
        extern glm::vec3 velocity;
        extern bool onGround;
    }

    extern int seed;
    extern string saveName;

    extern vector<glm::vec2> chunkMeshGenQueue;
    extern vector<glm::vec2> chunkMeshDelQueue;
}

#endif