#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "PhysicModel.h"

using namespace std;

class Player: public PhysicModel
{
public:
    Player(): PhysicModel("")
    {

    }

    Player(string path, glm::vec3 position = glm::vec3(0.0f)): PhysicModel(path)
    {
        setParametres(position);
    }

    void playerDraw(Shader& shader, GLfloat deltaTime)
    {
        setSpeed(deltaTime);
        cout << getSpeed().x << " " << getSpeed().y << " " << getSpeed().z << "\n";
        setTranslate(getSpeed() * deltaTime);
        Draw(shader);
    }

    void setTranslate(glm::vec3 a)
    {
        Model::setTranslate(a);
        this->camera.setTranslate(a);
    }

    void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        // setRotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(xoffset * this->camera.MouseSensitivity));
        this->camera.ProcessMouseMovement(xoffset, yoffset, constrainPitch);
    }

    void processKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        // setTranslate(this->camera.ProcessKeyboard(direction, deltaTime));
        glm::vec3 strenght;
        glm::vec3 front = this->camera.getFront();
        glm::vec3 right = this->camera.getRight();
        GLfloat velocity = this->movementSpeed;
        if (direction == FORWARD)
            strenght = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        if (direction == BACKWARD)
            strenght = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        if (direction == LEFT)
            strenght = glm::normalize(glm::vec3(right.x, 0.0f, right.z));
        if (direction == RIGHT)
            strenght = glm::normalize(glm::vec3(right.x, 0.0f, right.z));
        if (direction == UP)
            strenght = glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        strenght *= velocity;
        setBoost(strenght);
    }

    glm::vec3 getCameraPosition()
    {
        return this->camera.Position;
    }

    GLfloat getCameraZoom()
    {
        return this->camera.Zoom;
    }

    glm::mat4 getCameraViewMatrix()
    {
        return this->camera.GetViewMatrix();
    }

private:
    Camera camera;
    GLfloat movementSpeed;

    void setParametres(glm::vec3 position)
    {
        this->camera = Camera(position);
        this->movementSpeed = 20.0f;
    }
};

#endif