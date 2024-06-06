//
// Created by Dusha on 01/06/2024.
//
#pragma once
#include <glm/glm.hpp>

namespace rt {

    class Camera {

        static constexpr float CAM_MOV_SPEED = 0.007f;
        static constexpr float CAM_VIEW_SPEED = 0.001f;

    public:
        glm::vec3 m_camPos = glm::vec3(0.0f, 0.0f, -5.0f);
        glm::vec3 m_camDir = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::mat4 m_InvProjection;

        Camera(double fov, int width, int height, float near, float far);

        void moveForward(float deltaTime);
        void moveBackward(float deltaTime);
        void moveLeft(float deltaTime);
        void moveRight(float deltaTime);
        void updateCamDirection(float xoffset, float yoffset, float deltaTime);
    };

} // rt

