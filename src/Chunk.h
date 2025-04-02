#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FastNoise/FastNoiseLite.h>

#ifndef CHUNK_H
#define CHUNK_H

class Chunk {
    public:
        Chunk() {};

        glm::vec2 pos;

        static int chunkCoordToWorld (int cx, int cy, float x, float y, float z); //chunk x and y, and chunk coords x y z

        int blocks[16][16][64]; //xyz

        static Chunk genChunk (int cx, int cy);

        void setBlock (float x, float y, float z, int blockID); //in chunk coords
        int getBlock (float x, float y, float z);

        bool loaded;
};


#endif
