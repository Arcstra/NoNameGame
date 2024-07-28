#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
using namespace std;

vector<string> getElementsString(const string line, GLchar sep);

class Model
{
public:
    // model data 
    vector<Mesh>     meshes;
    map<string, Material> materials;
    string directory;

    // constructor, expects a filepath to a 3D model.
    Model(const string& path)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (GLuint i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
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