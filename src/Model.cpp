#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "World.h"

#include <iostream>
using namespace std;

Model::Model (const vector<float>& vVert, const vector<unsigned int>& vIndices, const vector<unsigned int>& vAttribLengths) : pos(glm::vec3(0)),rot(glm::vec2(0)) {
    setData(vVert, vIndices, vAttribLengths);

    // World::models.push_back(this); //Do this manually bc alway rebinds shader + want to do manually sometimes
}
void setData (const vector<float>& vVert, const vector<unsigned int>& vIndices, const vector<unsigned int>& vAttribLengths) {
    //Put vectors into arrays
    float v[vVert.size()];
    copy(vVert.begin(),vVert.end(), v);
    vertices = v;

    unsigned int i[vIndices.size()];
    copy(vIndices.begin(),vIndices.end(), i);
    indices = i;
    totalIndices = vIndices.size();

    unsigned int a[vAttribLengths.size()];
    copy(vAttribLengths.begin(),vAttribLengths.end(), a);
    attribLen = a;

    //get Stride and Offset for buffering
    unsigned int sums[vAttribLengths.size()]; //offset at j
    attrPerVert = 0; //Stride per vertex
    for (int j = 0; j < vAttribLengths.size(); j++) {
        sums[j] = attrPerVert;
        attrPerVert += a[j];
    }

    //Get total vertices
    int totalData = sizeof(v)/sizeof(v[0]);
    totalVertices = totalData/attrPerVert;


    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO); //editing VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i), i, GL_STATIC_DRAW);

    for (int j = 0; j < vAttribLengths.size(); j++) {
        glVertexAttribPointer(j, a[j], GL_FLOAT, GL_FALSE, attrPerVert * sizeof(float), (void*)(sums[j] * sizeof(float)));
		glEnableVertexAttribArray(j);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    dataFormatted = true;
}
void Model::draw () {
    if (!dataFormatted) return;
    shader->bind(*this);

    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(0x84C0+i);
        glBindTexture(GL_TEXTURE_2D, textures.at(i));
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,totalIndices,GL_UNSIGNED_INT, 0);
}
Model Model::joinModels(Model* models) {
    return models[0];
}