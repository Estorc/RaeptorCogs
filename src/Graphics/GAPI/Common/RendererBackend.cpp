#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/Renderer.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/RendererBackend.hpp>

namespace RaeptorCogs::GAPI::Common {

RendererBackend::~RendererBackend() {
    this->getObjectAllocator().destroyAll();
}

}