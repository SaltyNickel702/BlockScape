#include <map>
#include <string>
#include "Sound.h"

using namespace std;

#ifndef BLOCK_H
#define BLOCK_H

//Class that contains each block type's data
class Block {
    public:
        Block() {};
        Block(string name, int type) {
            displayName = name;
            blockID = type;
        }
        string displayName;
        int blockID;

        unsigned int textureSide;
        unsigned int textureTop;
        unsigned int textureBottom;

        bool tranparent;

        Sound* breakSound;
        Sound* placeSound;
};


#endif