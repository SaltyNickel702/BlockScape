#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

#ifndef MODEL_H
#define MODEL_H

class Model {
    public:
        Model () : pos(glm::vec3(0)),rot(glm::vec2(0)) {};
        Model (const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<unsigned int>& attribLengths);
        void setData();

        bool dataFormatted = false;
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

        glm::vec3 position;
        glm::vec2 rot;


        static Model joinModels (Model* models);
};

#endif