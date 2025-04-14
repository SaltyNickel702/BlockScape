#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FastNoise/FastNoiseLite.h>
#include "Model.h"

#ifndef CHUNK_H
#define CHUNK_H

//A chunk of the world which contains blocks and has its own mesh
class Chunk {
    public:
        Chunk();

        glm::vec2 pos;

        static int chunkCoordToWorld (int cx, int cz, float x, float y, float z); //chunk x and z, and chunk coords x y z

        int blocks[16][128][16]; //xyz

        static Chunk genChunk (int cx, int cz);

        void setBlock (int x, int y, int z, int blockID); //in chunk coords
        int* getBlock (int x, int y, int z);

        bool loaded;

        Model* mesh;
        void genMeshGL();
        void genMeshParam();

        vector<float> vertices;
        vector<unsigned int> indices;
        vector<unsigned int> attrib {3,3,2,1}; //pos, normal, uv, texture ID
};


#endif
