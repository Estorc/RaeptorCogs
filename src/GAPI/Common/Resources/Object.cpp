#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Object.hpp>

namespace RaeptorCogs::GAPI {

bool IsRendererBackendInitialized() {
    return RaeptorCogs::Renderer().isInitialized();
}

std::shared_ptr<RaeptorCogs::GAPI::Common::ObjectData> GetObjectData(std::type_index type) {
    return RaeptorCogs::Renderer().getBackend().Create(type);
}

}