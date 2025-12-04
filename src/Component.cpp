#include <RaeptorCogs/Component.hpp>

namespace RaeptorCogs {

void Component::setRenderer(Singletons::Renderer* renderer) {
    this->renderer = renderer;
}

Singletons::Renderer* Component::getRenderer() const {
    return this->renderer;
}

} // namespace RaeptorCogs