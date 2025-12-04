#include <RaeptorCogs/Time.hpp>
#include <GLFW/glfw3.h>

namespace RaeptorCogs::Singletons {

    void Time::computeDeltaTime() {
        this->deltaTime = glfwGetTime() - this->lastFrameTime;
        this->lastFrameTime = glfwGetTime();
    }
    double Time::getPreciseDeltaTime() {
        return this->deltaTime;
    }
    double Time::getPreciseTime() {
        return glfwGetTime();
    }
    float Time::getDeltaTime() {
        return static_cast<float>(this->deltaTime);
    }
    float Time::getTime() {
        return static_cast<float>(glfwGetTime());
    }

}