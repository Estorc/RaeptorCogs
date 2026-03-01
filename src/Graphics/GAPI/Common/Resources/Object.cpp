#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>

namespace RaeptorCogs::GAPI {

bool IsRendererBackendInitialized() {
  return RaeptorCogs::Renderer().isInitialized();
}

size_t ObjectAllocator::allocate(std::type_index type) {
  Common::ObjectData *object =
      RaeptorCogs::Renderer().getBackend().Create(type);
  if (object) {
    object->id = this->idAllocator.allocate(1);
    if (object->id == SIZE_MAX) {
      std::cerr << "Error: Maximum number of objects reached." << std::endl;
      delete object;
      return SIZE_MAX;
    }
    std::cout << "Allocated object of type " << type.name() << " with ID "
              << object->id << "." << std::endl;
    this->allocatedObjects.resize(
        std::max(this->allocatedObjects.size(), object->id + 1));
    this->allocatedObjects[object->id] = object;
    return object->id;
  }
  std::cerr << "Error: Failed to allocate object of type " << type.name() << "."
            << std::endl;
  return SIZE_MAX;
}

Common::ObjectData *ObjectAllocator::get(size_t id) {
  if (id < this->allocatedObjects.size()) {
    return this->allocatedObjects[id];
  }
  return nullptr;
}

void ObjectAllocator::destroy(size_t id) {
  if (id < this->allocatedObjects.size()) {
    Common::ObjectData *object = this->allocatedObjects[id];
    if (object) {
      delete object;
      this->allocatedObjects[id] = nullptr;
      this->idAllocator.free(id, id + 1);
      std::cout << "Destroyed object with ID " << id << "." << std::endl;
    } else {
      std::cerr << "Error: Attempted to destroy an invalid or already "
                   "destroyed object."
                << std::endl;
    }
  } else {
    std::cerr << "Error: Invalid object ID " << id << " for destruction."
              << std::endl;
  }
}

void ObjectAllocator::destroy(Common::ObjectData *object) {
  if (object) {
    size_t id = object->id;
    this->destroy(id);
  }
}

void ObjectAllocator::destroyAll() {
  GAPI::Common::RendererBackend &backend = RaeptorCogs::Renderer().getBackend();
  for (auto &object : this->allocatedObjects) {
    if (object) {
      std::cout << "Destroying object with ID " << object->id << "."
                << std::endl;
      delete object;
    }
  }
  this->allocatedObjects.clear();
  this->idAllocator.clear();
}

ObjectAllocator::ObjectAllocator() { idAllocator.free(0, MAX_OBJECTS); }

ObjectAllocator::~ObjectAllocator() { this->destroyAll(); }

size_t AllocateWithActiveBackend(std::type_index type) {
  return RaeptorCogs::Renderer().getBackend().getObjectAllocator().allocate(
      type);
}

Common::ObjectData *GetObjectDataFromIDWithActiveBackend(size_t id) {
  if (!RaeptorCogs::Renderer().isInitialized())
    return nullptr;
  Common::ObjectData *object =
      RaeptorCogs::Renderer().getBackend().getObjectAllocator().get(id);
  return object ? object : nullptr;
}

void FreeWithActiveBackend(size_t object) {
  RaeptorCogs::Renderer().getBackend().getObjectAllocator().destroy(object);
}

} // namespace RaeptorCogs::GAPI