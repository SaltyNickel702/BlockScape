#include <vector>
#include "Shader.h"

#ifndef MODEL_H
#define MODEL_H

class Model {
    public:
        Model (const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<unsigned int>& attribLengths);
        void draw ();

        float* vertices;
        unsigned int* indices;

        unsigned int* attribLen;

        int totalVertices;
        int attrPerVert;
        int totalIndices;

        unsigned int VAO, EBO, VBO;

        std::vector<unsigned int> textures;
        Shader* shader;


        static Model joinModels (Model models[]);
};

#endif