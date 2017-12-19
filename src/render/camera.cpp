#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace render {

Camera::Camera()
    : clamps { 0, 0 }
    , position { 0, 0, 0 }
    , direction { 0, 0, 1 }
    , yaw { 45 }
    , pitch { 0 }
{
    updateVectors();
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(position, position + direction, up);
}

void Camera::move(Camera::Direction d, float dt, Camera::altitude_func altitude) {
    float velocity = move_speed * dt;

    switch (d) {
        case Direction::Forward:
            position += direction * velocity;
            break;

        case Direction::Back:
            position -= direction * velocity;
            break;

        case Direction::Right:
            position += right * velocity;
            break;

        case Direction::Left:
            position -= right * velocity;
            break;
    }

    position.x = glm::clamp(position.x, 0.f, clamps[0]);
    position.z = glm::clamp(position.z, 0.f, clamps[1]);

    position.y = altitude(position.x, position.z) + 0.5;
}

void Camera::tilt(float dx, float dy) {
    dx *= turn_speed;
    dy *= turn_speed;

    yaw += dx;
    pitch = glm::clamp(pitch + dy, -89.0f, 89.0f);

    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 d = {
        std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
        std::sin(glm::radians(pitch)),
        std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
    };

    direction = glm::normalize(d);
    right     = glm::normalize(glm::cross(direction, glm::vec3 { 0, 1, 0 }));
    up        = glm::normalize(glm::cross(right, direction));
}

}
