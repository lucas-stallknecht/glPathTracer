//
// Created by Dusha on 01/06/2024.
//

#include "Camera.hpp"
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace rt {

    Camera::Camera(double fov, int width, int height, float near, float far) {

        glm::mat4 projection = glm::perspectiveFov(glm::radians(fov), (double)1280,  (double)720, 0.1, 1000.0);
        m_InvProjection = glm::inverse(projection);
    }

    void Camera::moveForward() {
        m_camPos += m_camDir * CAM_MOV_SPEED;
    }

    void Camera::moveBackward() {
        m_camPos -= m_camDir * CAM_MOV_SPEED;
    }

    void Camera::moveLeft() {
        m_camPos -= glm::cross(m_camDir, glm::vec3(0.0f, 1.0f, 0.0f)) * CAM_MOV_SPEED;
    }

    void Camera::moveRight() {
        m_camPos += glm::cross(m_camDir, glm::vec3(0.0f, 1.0f, 0.0f)) * CAM_MOV_SPEED;
    }

    void Camera::updateCamDirection(float deltax, float deltay){
        float pitchDelta = deltay * CAM_VIEW_SPEED;
        float yawDelta = deltax * CAM_VIEW_SPEED;

        glm::vec3 rightDirection = glm::cross(m_camDir, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::quat q = glm::normalize(glm::cross(glm::angleAxis(pitchDelta, rightDirection),
                                                glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
        m_camDir = glm::rotate(q, m_camDir);
    }

} // rt