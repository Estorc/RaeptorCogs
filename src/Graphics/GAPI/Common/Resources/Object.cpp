#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>

namespace RaeptorCogs::GAPI {

bool IsRendererBackendInitialized() {
    return RaeptorCogs::Renderer().isInitialized();
}

Common::ObjectData* ObjectAllocator::allocate(std::type_index type) {
    Common::ObjectData* object = RaeptorCogs::Renderer().getBackend().Create(type);
    if (object) {
        object->id = this->idAllocator.allocate(1);
        if (object->id == SIZE_MAX) {
            std::cerr << "Error: Maximum number of objects reached." << std::endl;
            delete object;
            return nullptr;
        }
        std::cout << "Allocated object of type " << type.name() << " with ID " << object->id << "." << std::endl;
        this->allocatedObjects.resize(std::max(this->allocatedObjects.size(), object->id + 1));
        this->allocatedObjects[object->id] = object;
        return object;
    }
    std::cerr << "Error: Failed to allocate object of type " << type.name() << "." << std::endl;
    return nullptr;
}

void ObjectAllocator::destroy(size_t id) {
    if (id < this->allocatedObjects.size()) {
        Common::ObjectData* object = this->allocatedObjects[id];
        if (object) {
            delete object;
            this->allocatedObjects[id] = nullptr;
            this->idAllocator.free(id, id + 1);
            std::cout << "Destroyed object with ID " << id << "." << std::endl;
        } else {
            std::cerr << "Error: Attempted to destroy an invalid or already destroyed object." << std::endl;
        }
    } else {
        std::cerr << "Error: Invalid object ID " << id << " for destruction." << std::endl;
    }
}

void ObjectAllocator::destroy(Common::ObjectData* object) {
    if (object) {
        size_t id = object->id;
        this->destroy(id);
    }
}


void ObjectAllocator::destroyAll() {
    GAPI::Common::RendererBackend& backend = RaeptorCogs::Renderer().getBackend();
    for (auto& object : this->allocatedObjects) {
        if (object) {
            std::cout << "Destroying object with ID " << object->id << "." << std::endl;
            delete object;
        }
    }
    this->allocatedObjects.clear();
    this->idAllocator.clear();
}

ObjectAllocator::ObjectAllocator() {
    idAllocator.free(0, MAX_OBJECTS);
}

ObjectAllocator::~ObjectAllocator() {
    this->destroyAll();
}

Common::ObjectData* AllocateWithActiveBackend(std::type_index type) {
    return RaeptorCogs::Renderer().getBackend().getObjectAllocator().allocate(type);
}

void FreeWithActiveBackend(Common::ObjectData* object) {
    RaeptorCogs::Renderer().getBackend().getObjectAllocator().destroy(object);
}

}