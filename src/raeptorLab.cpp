#include <RaeptorLab/raeptorLab.hpp>

::Renderer& RaeptorLab::Renderer() {
    static ::Renderer instance;
    return instance;
}