#ifndef Collision_H
#define Collision_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

using namespace std;

bool isPointInCollision(glm::vec3 point, glm::vec3* vertex, glm::vec3 centre);

class CollisionRectangle
{
public:

    CollisionRectangle(glm::vec3 vertex[8])
    {
        setParametres(vertex);
    }

    glm::vec3* getVertex() {
        return this->vertex;
    }

    glm::vec3 getCentre() {
        return this->centre;
    }

    void setModel(glm::mat4& model) {
        for (int i = 0; i < 8; ++i)
            vertex[i] = model * glm::vec4(constVertex[i].x, constVertex[i].y, constVertex[i].z, 1.0f);
    }

    glm::vec3 vecIntersection(CollisionRectangle other)
    {
        /*
        Возвращает вектор из области пересечения
        в центр фигуры a.
        */
        glm::vec3* vertexO = other.getVertex();
        glm::vec3 centreO = other.getCentre();
        glm::vec3 centreIn(0.0f);
        GLuint countIn = 0;

        for (GLuint i = 0; i < 8; ++i) {
            if (isPointInCollision(vertex[i], vertexO, centreO)) {
                centreIn += vertexO[i];
                countIn++;
            }
        }

        if (!countIn)
            return glm::vec3(0.0f);

        cout << countIn << "\n";

        centreIn /= (GLfloat)countIn;

        return centre - centreIn;
    }

private:
    // A B C D A1 B1 C1 D1
    glm::vec3 constVertex[8];
    glm::vec3 vertex[8];
    glm::vec3 centre;

    void setParametres(glm::vec3 vertex[8])
    {
        *this->constVertex = *vertex;
        this->centre = vertex[0] + (vertex[6] - vertex[0]) / 2.0f;
    }

};

class CollisionSphere
{
public:

    CollisionSphere(glm::vec3 centre, GLfloat radius)
    {
        cout << centre.y << "\n" << radius << "\n";
        setParametres(centre, radius);
    }

    glm::vec3 getCentre() {
        return this->centre;
    }

    GLfloat getRadius() {
        return this->radius;
    }

    void setModel(glm::mat4& model) {
        centre = model * glm::vec4(constCentre.x, constCentre.y, constCentre.z, 1.0f);
    }

    glm::vec3 vecIntersection(CollisionSphere other)
    {
        glm::vec3 otherCentre = other.getCentre();
        GLfloat otherRadius = other.getRadius();
        GLfloat len = otherRadius + radius - glm::length(centre - otherCentre);

        if (len < 0)
            return glm::vec3(0.0f, 0.0f, 0.0f);
            
        return normalize(centre - otherCentre);
    }

private:
    // A B C D A1 B1 C1 D1
    glm::vec3 constCentre, centre;
    GLfloat radius;

    void setParametres(glm::vec3 centre, GLfloat radius)
    {
        this->constCentre = centre;
        this->radius = radius;
    }

};

bool isPointInCollision(glm::vec3 point, glm::vec3* vertex, glm::vec3 centre)
{
    for (GLuint i = 0; i < (GLuint)(sizeof(vertex)/sizeof(*vertex)); ++i) {
        if (glm::dot(centre - point, vertex[i]) <= 0)
            return false;
    }

    return true;
}

#endif