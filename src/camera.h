#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <functional>

namespace graphics {

class Camera {
public:
    enum class Direction {
        Forward,
        Back,
        Left,
        Right,
    };

    Camera();

    void setClamps(glm::vec2 clamps) {
        this->clamps = clamps;
    }

    glm::mat4 getView() const;

    using altitude_func = std::function<float(float, float)>;
    void move(Direction d, float dt, altitude_func altitude);
    void tilt(float dx, float dy);

private:
    glm::vec2 clamps;

    glm::vec3 position;

    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;
    float yaw;
    float pitch;

    static constexpr float move_speed = 1;
    static constexpr float turn_speed = 0.1;

    void updateVectors();
};

}

#endif
