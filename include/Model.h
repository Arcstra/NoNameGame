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
    Model(string path)
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
        this->translate += a;
        setModel();
        setCollisionModel();
    }

    void setScale(glm::vec3 a)
    {
        this->scale *= a;
        setModel();
        setCollisionModel();
    }

    void setRotate(glm::vec3 a, GLfloat angle)
    {
        glm::vec3 point0, point1;
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, a);
        angle = 0.0f;
        glm::vec3 croosAngle = glm::cross(glm::vec3(1.0f), glm::vec3(model * glm::vec4(1.0f)));

        point0 = glm::vec3(1.0f, 1.0f, 0.0f);
        point1 = model * glm::vec4(point0.x, point0.y, point0.z, 1.0f);
        angle = glm::acos(glm::dot(glm::normalize(glm::vec2(point0.x, point0.y)),glm::normalize(glm::vec2(point1.x, point1.y))));
        if (croosAngle.z < 0.0f)
            angle *= -1;
        this->rotateXY = angle;

        point0 = glm::vec3(1.0f, 0.0f, 1.0f);
        point1 = model * glm::vec4(point0.x, point0.y, point0.z, 1.0f);
        angle = glm::acos(glm::dot(glm::normalize(glm::vec2(point0.z, point0.x)),glm::normalize(glm::vec2(point1.z, point1.x))));
        if (croosAngle.y < 0.0f)
            angle *= -1;
        this->rotateZX = angle;

        point0 = glm::vec3(0.0f, 1.0f, 1.0f);
        point1 = model * glm::vec4(point0.x, point0.y, point0.z, 1.0f);
        angle = glm::acos(glm::dot(glm::normalize(glm::vec2(point0.z, point0.y)),glm::normalize(glm::vec2(point1.z, point1.y))));
        if (croosAngle.x < 0.0f)
            angle *= -1;
        this->rotateZY = angle;
        
        setModel();
        setCollisionModel();
    }

    void addCollisionRectangle(vector<glm::vec3> vertex)
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
    GLfloat rotateXY, rotateZY, rotateZX;
    glm::vec3 translate, scale;

    void setOneModel()
    {
        this->translate = glm::vec3(0.0f);
        this->scale = glm::vec3(1.0f);
        this->uniqueNumber = uuid::generate_uuid_v4();
        this->rotateXY = 0.0f;
        this->rotateZX = 0.0f;
        this->rotateZY = 0.0f;
        setModel();
        setCollisionModel();
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

    void setModel()
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, translate);
        model = glm::rotate(model, rotateXY, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, rotateZX, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotateZY, glm::vec3(1.0, 0.0f, 0.0f));
        model = glm::scale(model, scale);
        this->model = model;
    }

    void setCollisionModel()
    {
        for (CollisionRectangle& col: colrec)
            col.setModel(this->model);

        for (CollisionSphere& col: sphereCollisions)
            col.setModel(this->model);
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