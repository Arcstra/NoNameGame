#ifndef Collision_H
#define Collision_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

using namespace std;

bool isPointInCollision(glm::vec3 point, vector<glm::vec3> vertex, glm::vec3 centre);
glm::vec3 Nearest2Segment( const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c );


class CollisionRectangle
{
public:

    CollisionRectangle(vector<glm::vec3> vertex)
    {
        setParametres(vertex);
    }

    vector<glm::vec3> getVertex() {
        return this->vertex;
    }

    glm::vec3 getCentre() {
        return this->centre;
    }

    void setModel(glm::mat4& model) {
        for (int i = 0; i < 8; ++i)
            vertex[i] = model * glm::vec4(constVertex[i].x, constVertex[i].y, constVertex[i].z, 1.0f);
        centre = model * glm::vec4(constCentre.x, constCentre.y, constCentre.z, 1.0f);
    }

private:
    // A B C D A1 B1 C1 D1
    vector<glm::vec3> constVertex;
    vector<glm::vec3> vertex;
    glm::vec3 constCentre, centre;

    void setParametres(vector<glm::vec3> vertex)
    {
        this->constVertex = vertex;
        this->vertex = vertex;
        this->constCentre = vertex[0] + (vertex[6] - vertex[0]) / 2.0f;
        this->centre = vertex[0] + (vertex[6] - vertex[0]) / 2.0f;
    }

};

class CollisionSphere
{
public:

    CollisionSphere(glm::vec3 centre, GLfloat radius)
    {
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

class CollisionFace
{
public:
    CollisionFace(vector<glm::vec3> points)
    {
        setParametres(points);
    }

    vector<glm::vec3> getPoints()
    {
        return this->points;
    }

    glm::vec3 getCentre()
    {
        return this->centre;
    }

private:
    vector<glm::vec3> points;
    glm::vec3 centre;

    void setParametres(vector<glm::vec3> points)
    {
        this->points = points;
        glm::vec3 now(0.0f);
        for (GLuint i = 0; i < points.size(); ++i)
            now += points[i];
        this->centre = now / (GLfloat)points.size();
    }
};

class CollisionMesh
{
public:
    CollisionMesh(vector<CollisionFace> faces)
    {
        setParametres(faces);
    }

    vector<CollisionFace> getFaces()
    {
        return this->faces;
    }

    glm::vec3 getCentre()
    {
        return this->centre;
    }

private:
    vector<CollisionFace> faces;
    glm::vec3 centre;

    void setParametres(vector<CollisionFace> faces)
    {
        this->faces = faces;
        glm::vec3 now(0.0f);
        for (GLuint i = 0; i < faces.size(); ++i)
            now += faces[i].getCentre();
        this->centre = now / (GLfloat)faces.size();
    }
};

struct Line {
    glm::vec3 start, finish;
};

bool isPointInCollision(glm::vec3 point, vector<glm::vec3> vertex, glm::vec3 centre)
{
    for (GLuint i = 0; i < vertex.size(); ++i) {
        if (glm::dot(centre - point, vertex[i] - point) <= 0)
            return true;
    }

    return false;
}

bool isRayToLine(glm::vec3 point, glm::vec3 dir, Line line)
{
    GLfloat grad0 = glm::acos(glm::dot(glm::normalize(line.start - point), glm::normalize(line.finish - point)));
    GLfloat grad1 = glm::acos(glm::dot(glm::normalize(line.start - point), glm::normalize(dir)));
    GLfloat grad2 = glm::acos(glm::dot(glm::normalize(dir), glm::normalize(line.finish - point)));

    // cout << "isRayToLine: \n";
    // cout << dir.x << " " << dir.y << " " << dir.z << "\n";
    // cout << grad0 << " " << grad1 << " " << grad2 << "\n";
    // cout << glm::abs(grad0 - grad1 - grad2) << "\n";

    return glm::abs(grad0 - grad1 - grad2) < 0.00001f;
}

bool isPointInFace(glm::vec3 point, CollisionFace face)
{
    // cout << "isPointInFace\n";
    vector<glm::vec3> vertex = face.getPoints();
    glm::vec3 dir = glm::normalize(vertex[1] - vertex[0]);
    GLuint k = 0;
    vertex.push_back(vertex[0]);
    for (GLuint i = 1; i < vertex.size(); ++i) {
        k += isRayToLine(point, dir, Line{vertex[i - 1], vertex[i]});
        // cout << vertex[i].x << " " << vertex[i].y << " " << vertex[i].z << "\n";
    }
    // cout << k << "\n";
    return k % 2;
}

bool isRayFace(glm::vec3 point, glm::vec3 dir, CollisionFace face)
{
    // cout << "isRayFace\n";
    dir = glm::normalize(dir);
    vector<glm::vec3> vertex = face.getPoints();
    glm::vec3 normal = glm::cross(vertex[3] - vertex[0], vertex[1] - vertex[0]);
    if ((normal.x * dir.x + normal.y * dir.y + normal.z * dir.z) == 0) return false;
    GLfloat D = -(normal.x * vertex[0].x + normal.y * vertex[0].y + normal.z * vertex[0].z);
    GLfloat k = -(normal.x * point.x + normal.y * point.y + normal.z * point.z + D) / (normal.x * dir.x + normal.y * dir.y + normal.z * dir.z);
    // cout << k << "\n";
    if (k < 0.0f) return false;

    return isPointInFace(point + dir * k, face);
}

bool isPointInMesh(glm::vec3 point, CollisionMesh mesh)
{
    // cout << "isPointInMesh\n";
    glm::vec3 dir(1.0f, 0.0f, 0.0f);
    vector<CollisionFace> faces = mesh.getFaces();
    GLuint k = 0;
    for (GLuint i = 0; i < faces.size(); ++i) {
        k += isRayFace(point, dir, faces[i]);
    }
    // cout << k << "\n";
    return k % 2;
}

glm::vec3 Nearest2Segment( const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c )
{
  glm::vec3 vecAB = b - a;
  glm::vec3 vecAC = c - a;
 
// относительная длина проекции AC на AB
  float relativeD = glm::dot(vecAB, vecAC) * glm::length(vecAC) / glm::length(vecAB);
 
// относительная длина < 0, ближайшая A
  if (relativeD < 0.0f) return a;
 
// относительная длина > 1, ближайшая B
  if (relativeD > 1.0f) return b;
 
// возвращаем точку проекции
  return a + vecAB * relativeD;
}

glm::vec3 vecIntersection(CollisionRectangle a, CollisionRectangle b)
{
    /*
    Возвращает вектор из области пересечения
    в центр фигуры a.
    */
    vector<glm::vec3> vertexA = a.getVertex();
    vector<glm::vec3> vertexB = b.getVertex();
    glm::vec3 centreA = a.getCentre();
    GLuint indicesFace[] = {
        0, 1, 2, 3,
        1, 0, 4, 5,
        2, 1, 5, 6,
        3, 2, 6, 7,
        3, 0, 7, 4,
        5, 4, 7, 6
    };
    vector<CollisionFace> faces;
    for (GLuint i = 0; i < 24; i += 4)
        faces.push_back(CollisionFace{vector{vertexB[indicesFace[i]], vertexB[indicesFace[i + 1]], vertexB[indicesFace[i + 2]], vertexB[indicesFace[i + 3]]}});

    vector<glm::vec3> vertexIn;
    for (GLuint i = 0; i < 8; ++i) {
        // cout << vertexA[i].x << " " << vertexA[i].y << " " << vertexA[i].z << "\n";
        if (isPointInMesh(vertexA[i], CollisionMesh{faces})) {
            vertexIn.push_back(vertexA[i]);
        }
        // cout << "\n";
    }

    if (vertexIn.empty())
        return glm::vec3(0.0f);

    // cout << "VertexIn: " << vertexIn.size() << "\n";

    // if (vertexIn.size() % 2 == 0) {
        glm::vec3 point(0.0f);
        glm::vec3 nearVector(0.0f);

        for (GLuint i = 0; i < vertexIn.size(); ++i)
            point += vertexIn[i];
        point /= (GLfloat)vertexIn.size();

        // cout << point.x << " " << point.y << " " << point.z << "\n";

        for (GLuint i = 0; i < 24; i += 4) {
            vector<glm::vec3> points = {vertexA[indicesFace[i]], vertexA[indicesFace[i + 1]], vertexA[indicesFace[i + 2]], vertexA[indicesFace[i + 3]]};
            if (isRayFace(point, centreA - point, CollisionFace{points}))
                nearVector = glm::normalize(glm::cross(points[3] - points[0], points[1] - points[0]));
        }

        return nearVector;
    // }

    return glm::vec3(0.0f);
}

glm::vec3 vecIntersection(CollisionRectangle a, CollisionSphere b)
{
    /*
    Возвращает вектор из области пересечения
    в центр фигуры a.
    */
    vector<glm::vec3> vertex = a.getVertex();
    glm::vec3 centre = a.getCentre();
    glm::vec3 centreO = b.getCentre();
    GLfloat radiusO = b.getRadius();
    glm::vec3 normal, point, nearVector(1000000000.0f);
    GLfloat D, k;

    GLuint indicesFace[] = {
        0, 1, 2, 3,
        1, 0, 4, 5,
        2, 1, 5, 6,
        3, 2, 6, 7,
        3, 0, 7, 4,
        5, 4, 7, 6
    };

    for (GLuint i = 0; i < 24; i += 4) {
        normal = glm::normalize(glm::cross(vertex[indicesFace[i + 1]] - vertex[indicesFace[i]], vertex[indicesFace[i + 3]] - vertex[indicesFace[i]]));
        D = -(vertex[indicesFace[i]].x * normal.x + vertex[indicesFace[i]].y * normal.y + vertex[indicesFace[i]].z * normal.z);
        k = -(normal.x * centreO.x + normal.y * centreO.y + normal.z * centreO.z + D);
        vector<glm::vec3> face = {vertex[indicesFace[i]], vertex[indicesFace[i + 1]], vertex[indicesFace[i + 2]], vertex[indicesFace[i + 3]]};
        if (isPointInFace(centreO + normal * k, CollisionFace{face}) && glm::length(normal * k) < glm::length(nearVector)) {
            nearVector = normal * k;
        }
    }

    GLuint indicesEdge[24] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        4, 5,
        5, 6,
        6, 7,
        7, 4,
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };

    for (GLuint i = 0; i < 24; i += 2) {
        glm::vec3 vec = Nearest2Segment(vertex[indicesEdge[i]], vertex[indicesEdge[i + 1]], centreO) - centreO;

        if (glm::length(vec) < glm::length(nearVector))
            nearVector = vec;
    }

    // cout << nearVector.x << " " << nearVector.y << " " << nearVector.z << "\n";

    if (glm::length(nearVector) < radiusO)
        return glm::normalize(nearVector);
    
    return glm::vec3(0.0f);
}

glm::vec3 vecIntersection(CollisionSphere a, CollisionSphere b)
{
    glm::vec3 centre = a.getCentre();
    GLfloat radius = a.getRadius();
    glm::vec3 otherCentre = b.getCentre();
    GLfloat otherRadius = b.getRadius();
    GLfloat len = otherRadius + radius - glm::length(centre - otherCentre);

    if (len < 0)
        return glm::vec3(0.0f, 0.0f, 0.0f);
        
    return normalize(centre - otherCentre);
}

#endif