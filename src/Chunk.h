#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FastNoise/FastNoiseLite.h>
#include "Model.h"

#ifndef CHUNK_H
#define CHUNK_H

class Chunk {
    public:
        Chunk() : loaded(false) {mesh = new Model();};

        glm::vec2 pos;

        static int chunkCoordToWorld (int cx, int cz, float x, float y, float z); //chunk x and z, and chunk coords x y z

        int blocks[16][64][16]; //xyz

        static Chunk genChunk (int cx, int cz);

        void setBlock (int x, int y, int z, int blockID); //in chunk coords
        int* getBlock (int x, int y, int z);

        bool loaded;

        Model* mesh;
        Model genMesh();

};


#endif
