#include <map>
#include <string>

using namespace std;

#ifndef BLOCK_H
#define BLOCK_H

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
};


#endif