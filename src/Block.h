#include <map>
#include <string>

using namespace std;

#ifndef BLOCK_H
#define BLOCK_H

class Block {
    public:
        Block(string name, float type) {displayName = name; blockID = type;}
        string displayName;
        int blockID;
};


#endif