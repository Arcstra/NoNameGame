#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;

    bool operator ==(const Vertex& x)
    {
        return (Position == x.Position && Normal == x.Normal);
    }
};

struct Material {
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    GLfloat Shininess;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex> vertices;
    vector<GLuint> indices;
    Material material;
    GLuint VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, Material material)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->material = material;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader& shader)
    {
        shader.setVec3("material.ambient", material.Ambient);
        shader.setVec3("material.diffuse", material.Diffuse);
        shader.setVec3("material.specular", material.Specular);
        shader.setFloat("material.shininess", material.Shininess);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // render data 
    GLuint VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};
#endif