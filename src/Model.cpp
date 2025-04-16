#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "World.h"

#include <iostream>
using namespace std;

Model::Model (const vector<float>& vVert, const vector<unsigned int>& vIndices, const vector<unsigned int>& vAttribLengths) : pos(glm::vec3(0)),rot(glm::vec2(0)), VAO(0), VBO(0), EBO(0), dataFormatted(false) {
    setData(vVert, vIndices, vAttribLengths);

    // World::models.push_back(this); //Do this manually bc alway rebinds shader + want to do manually sometimes
}
void Model::setData(const vector<float>& vVert, const vector<unsigned int>& vIndices, const vector<unsigned int>& vAttribLengths) {
    cleanData();

    // Store raw data
    verticesVec = vVert;
    indicesVec = vIndices;
    attribLenVec = vAttribLengths;

    float* v = verticesVec.data();
    unsigned int* i = indicesVec.data();
    unsigned int* a = attribLenVec.data();

    totalIndices = indicesVec.size();

    // Calculate attribute layout
    attrPerVert = 0;
    vector<unsigned int> sums(attribLenVec.size()); //Stride per vertex
    for (int j = 0; j < attribLenVec.size(); j++) {
        sums[j] = attrPerVert;
        attrPerVert += a[j];
    }

    totalVertices = verticesVec.size() / attrPerVert;

    // Generate OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesVec.size() * sizeof(float), v, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesVec.size() * sizeof(unsigned int), i, GL_STATIC_DRAW);

    for (int j = 0; j < attribLenVec.size(); j++) {
        glVertexAttribPointer(j, a[j], GL_FLOAT, GL_FALSE, attrPerVert * sizeof(float), (void*)(sums[j] * sizeof(float)));
        glEnableVertexAttribArray(j);
    }

    glBindVertexArray(0);
    dataFormatted = true;
}
void Model::draw () {
    if (!dataFormatted || VAO == 0 || totalIndices == 0) return;
    glUseProgram(shader->ID);
    shader->uniforms(pos,rot);

    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, textures.at(i));
        string texName = "tex" + to_string(i);
        glUniform1i(glGetUniformLocation(shader->ID,texName.c_str()),i);
    }


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,totalIndices,GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
Model Model::joinModels (Model* models) {
    return models[0];
}
void Model::cleanData () {
    if (!dataFormatted) return;
    dataFormatted = false;
    if (glIsVertexArray(VAO)) glDeleteVertexArrays(1, &VAO);
    if (glIsBuffer(VBO)) glDeleteBuffers(1, &VBO);
    if (glIsBuffer(EBO)) glDeleteBuffers(1, &EBO);
}