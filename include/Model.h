#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Collision.h"
#include "uuid.h"

#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;



vector<string> getElementsString(const string line, GLchar sep);

class Model
{
public:
    // constructor, expects a filepath to a 3D model.
    Model(const string& path)
    {
        loadModel(path);
        setOneModel();
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        shader.setMat4("model", model);
        for (GLuint i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    void setTranslate(glm::vec3 a)
    {
        model = glm::translate(model, a);
        SetCollisionModel();
    }

    void setScale(glm::vec3 a)
    {
        model = glm::scale(model, a);
        SetCollisionModel();
    }

    void setRotate(glm::vec3 a, GLfloat angle)
    {
        model = glm::rotate(model, angle, a);
        SetCollisionModel();
    }

    void addCollisionRectangle(glm::vec3 vertex[8])
    {
        colrec.push_back(CollisionRectangle(vertex));
    }

    vector<CollisionRectangle> getCollisionRectangle()
    {
        return colrec;
    }

    void addCollisionSphere(glm::vec3 centre, GLfloat radius)
    {
        sphereCollisions.push_back(CollisionSphere(centre, radius));
    }

    vector<CollisionSphere> getCollisionSphere()
    {
        return sphereCollisions;
    }

    string getUniqueNumber()
    {
        return this->uniqueNumber;
    }

private:
    // model data 
    vector<Mesh> meshes;
    vector<CollisionRectangle> colrec;
    vector<CollisionSphere> sphereCollisions;
    map<string, Material> materials;
    string directory;
    glm::mat4 model;
    string uniqueNumber;

    void setOneModel()
    {
        this->model = glm::mat4(1.0f);
        this->model = glm::translate(this->model, glm::vec3(0.0f, 0.0f, 0.0f));
        this->model = glm::scale(this->model, glm::vec3(1.0f, 1.0f, 1.0f));
        this->uniqueNumber = uuid::generate_uuid_v4();
        SetCollisionModel();
    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const string& path)
    {
        vector<string> lines;
        string line;
        string directory = path.substr(0, path.find_last_of('/')) + "/";
        string nameMaterial;

        vector<glm::vec3> vertex;
        vector<glm::vec3> normal;
        vector<Vertex> vertices;
        vector<GLuint> indices;
        
        ifstream in(path);
        if (in.is_open())
            while (getline(in, line))
                lines.push_back(line);
        in.close();

        for (string line: lines) {
            vector<string> elements = getElementsString(line, ' ');

            if (elements[0] == "o") {
                if (vertices.empty())
                    continue;

                this->meshes.push_back(Mesh(vertices, indices, this->materials[nameMaterial]));
                vertices.clear();
                indices.clear();
            }
            else if (elements[0] == "mtllib")
                setMaterials(directory + elements[1]);
            else if (elements[0] == "v")
                vertex.push_back(glm::vec3(stof(elements[1]), stof(elements[2]), stof(elements[3])));
            else if (elements[0] == "vn")
                normal.push_back(glm::vec3(stof(elements[1]), stof(elements[2]), stof(elements[3])));
            else if (elements[0] == "usemtl")
                nameMaterial = elements[1];
            else if (elements[0] == "f") {
                for (GLuint i = 1; i < 4; ++i) {
                    vector<string> a = getElementsString(elements[i], '/');

                    Vertex now;
                    now.Position = vertex[stoi(a[0]) - 1];
                    now.Normal = normal[stoi(a[1]) - 1];

                    auto search = find(vertices.begin(), vertices.end(), now);

                    if (search == vertices.end()) {
                        indices.push_back(vertices.size());
                        vertices.push_back(now);
                    }
                    else
                        indices.push_back(search - vertices.begin());
                }
            }
        }

        if (!vertices.empty())
            this->meshes.push_back(Mesh(vertices, indices, this->materials[nameMaterial]));
    }

    void setMaterials(const string& path) {
        map<string, Material> materials;
        string line, nameMaterial;
        vector<string> lines;

        ifstream in(path);
        if (in.is_open())
            while (getline(in, line))
                lines.push_back(line);
        in.close();

        for (string line: lines) {
            vector<string> elements = getElementsString(line, ' ');

            if (elements[0] == "newmtl")
                nameMaterial = elements[1];
            else if (elements[0] == "Ns")
                materials[nameMaterial].Shininess = stof(elements[1]);
            else if (elements[0] == "Ka")
                materials[nameMaterial].Ambient = glm::vec3(stof(elements[1]), stof(elements[2]), stof(elements[3]));
            else if (elements[0] == "Kd")
                materials[nameMaterial].Diffuse = glm::vec3(stof(elements[1]), stof(elements[2]), stof(elements[3]));
            else if (elements[0] == "Ks")
                materials[nameMaterial].Specular = glm::vec3(stof(elements[1]), stof(elements[2]), stof(elements[3]));
        }

        this->materials = materials;
    }

    void SetCollisionModel() {
        for (CollisionRectangle& col: colrec)
            col.setModel(model);

        for (CollisionSphere& col: sphereCollisions)
            col.setModel(model);
    }
};


inline vector<string> getElementsString(const string line, GLchar sep)
{
    vector<string> ans;
    string now = "";

    for (GLchar c : line) {
        if (c == sep) {
            if (!now.empty()) {
                ans.push_back(now);
                now = "";
            }
        }
        else {
            now.push_back(c);
        }
    }

    if (!now.empty())
        ans.push_back(now);

    if (ans.empty())
        ans.push_back("");

    return ans;
}

#endif