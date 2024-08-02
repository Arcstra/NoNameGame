#ifndef PhysicModel_H
#define PhysicModel_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"
#include "Collision.h"

#include <unordered_map>

using namespace std;

class StaticModel: public Model
{
public:
    StaticModel(const string& path, GLfloat weight = 1.0f, GLfloat energyCoefficient = 0.8f): Model(path)
    {
        setParametres(weight, energyCoefficient);
    }

    void StaticDraw(Shader& shader)
    {
        Draw(shader);
    }

    GLfloat getEnergyCoefficient()
    {
        return this->energyCoefficient;
    }

private:
    GLfloat weight, energyCoefficient;

    void setParametres(GLfloat weight = 1.0f, GLfloat energyCoefficient = 0.99f)
    {
        this->weight = weight;
        this->energyCoefficient = energyCoefficient;
    }
};

class PhysicModel: public Model
{
public:
    PhysicModel(const string& path, GLfloat weight = 1.0f): Model(path)
    {
        setParametres(weight);
    }

    void PhysicDraw(Shader& shader, GLfloat& delta)
    {
        setSpeed(delta);
        cout << speed.y << " " << getCollisionSphere()[0].getCentre().y << "\n";
        setTranslate(this->speed * delta);
        Draw(shader);
    }

    void setBoostWithCollisionRectangle(Model& other)
    {
        glm::vec3 strenght(0.0f);
        vector<CollisionRectangle> collisions = other.getCollisionRectangle();
        vector<CollisionRectangle> my_collisions = getCollisionRectangle();

        for (CollisionRectangle& my_collision: my_collisions) {
            for (CollisionRectangle& other_collision: collisions) {
                strenght += my_collision.vecIntersection(other_collision);
            }
        }

        boost += strenght / weight;
    }

    void setBoostWithCollisionSphere(StaticModel& other)
    {
        glm::vec3 strenght(0.0f);
        vector<CollisionSphere> otherCollisions = other.getCollisionSphere();
        vector<CollisionSphere> myCollisions = getCollisionSphere();

        for (CollisionSphere& myCollision: myCollisions) {
            for (CollisionSphere& otherCollision: otherCollisions) {
                strenght += myCollision.vecIntersection(otherCollision);
            }
        }

        if (strenght != glm::vec3(0.0f)){
            GLfloat coef = other.getEnergyCoefficient();
            strenght = glm::normalize(strenght);
            speed = strenght * glm::length(speed) * coef;
            boost += strenght * glm::length(const_boost) * glm::dot(strenght, glm::vec3(0.0f, 1.0f, 0.0f)) / weight;
        }
    }

private:
    glm::vec3 const_boost, boost, speed;
    GLfloat weight;

    void setParametres(GLfloat weight = 1.0f)
    {
        const_boost = glm::vec3(0.0f, -9.8f, 0.0f);
        boost = glm::vec3(0.0f, 0.0f, 0.0f);
        speed = glm::vec3(0.0f);
        this->weight = weight;
    }

    void setSpeed(GLfloat delta)
    {
        speed += (const_boost + boost) * delta;
        boost = glm::vec3(0.0f);
    }
};

#endif