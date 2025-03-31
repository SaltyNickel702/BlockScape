#include <map>
#include <string>

using namespace std;

#ifndef BLOCK_H
#define BLOCK_H

class Block {
    public:
        Block(string name, float type) {displayName = name; blockType = type;}
        string displayName;
        float blockType;
};


#endif