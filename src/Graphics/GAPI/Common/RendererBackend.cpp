#include <RaeptorCogs/Graphics/GAPI/Common/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/Renderer.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>

namespace RaeptorCogs::GAPI::Common {

RendererBackend::~RendererBackend() {
  this->getObjectAllocator().destroyAll();
}

} // namespace RaeptorCogs::GAPI::Common