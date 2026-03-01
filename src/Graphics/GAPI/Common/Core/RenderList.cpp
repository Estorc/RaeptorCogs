#include <RaeptorCogs/External/glad/glad.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Core/RenderList.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/Graphic.hpp>


namespace RaeptorCogs::GAPI::Common {

constexpr int INDEX_INDIRECTION_SIZE = (MAX_SPRITES * sizeof(int));

GraphicBatchHandler &RenderList::getHandler(size_t index) {
  return batch[index];
}

GraphicBatchHandler &RenderList::getIndirectHandler(size_t index) {
  return batch[orderIndices[index]];
}

GraphicBatchHandler &RenderList::createHandler(
    BatchKey key, Graphic2D *graphic, InstanceAllocator &instanceAllocator) {
  graphic->setBatchHandlerCursor(this->batch.size());
  RaeptorCogs::GAPI::Common::GraphicBatchHandler *handler;
  if (graphic->getRenderListCount() == 0) {
    handler = &this->batch.emplace_back(key, graphic);
    graphic->computeInstanceData(
        instanceAllocator, ComputeInstanceDataMode::FORCE_REBUILD);
    // Move the handler to fill any gaps if needed
    if (handler->staticDataCursor < this->batch.size() - 1) {
      this->batch[handler->staticDataCursor] = *handler;
      this->batch.pop_back();
      handler = &this->batch[handler->staticDataCursor];
      graphic->setBatchHandlerCursor(handler->staticDataCursor);
    }
    this->orderIndices.push_back(handler->staticDataCursor);
    if ((this->size() > 1 &&
         this->getIndirectHandler(this->orderIndices.size() - 2).rendererKey >
             handler->rendererKey) ||
        this->needsReorder()) {
      this->markDirty(*handler);
    }
    this->flags |= RenderListFlags::REORDERED;
  } else {
    handler = &graphic->getBatchHandler();
  }
  graphic->getRenderLists().push_back(this);
  return *handler;
}

RenderList::RenderList(BatchBuffer &batch) : batch(batch), flags(RenderListFlags::NONE) {}

void RenderList::bind() {
  if (!(flags & RenderListFlags::SSBO_CREATED)) {
    this->indexIndirectionSSBO->bind();
    this->indexIndirectionSSBO->allocate(INDEX_INDIRECTION_SIZE);
    this->flags |= RenderListFlags::SSBO_CREATED;
  }
  this->indexIndirectionSSBO->setBindingPoint(0);
}

bool RenderList::empty() const {
  return orderIndices.empty();
}

void RenderList::clear() {
  orderIndices.clear();
  flags = RenderListFlags::NONE;
}

void RenderList::erase(
    GraphicBatchHandler &handler, InstanceAllocator &instanceAllocator) {
  size_t index      = static_cast<size_t>(&handler - &batch[0]);
  auto &renderLists = handler.graphic->getRenderLists();
  renderLists.erase(
      std::remove(renderLists.begin(), renderLists.end(), this), renderLists.end());
  if (handler.graphic->getRenderListCount() == 0) {
    instanceAllocator.free(handler);
    handler.graphic->setBatchHandlerCursor(SIZE_MAX);
  }
  orderIndices.erase(
      std::remove(orderIndices.begin(), orderIndices.end(), index), orderIndices.end());
  this->flags |= RenderListFlags::REORDERED;
}

bool RenderList::needsReorder() const {
  return RenderListFlags::NEEDS_REORDER == (flags & RenderListFlags::NEEDS_REORDER);
}

bool RenderList::wasReordered() const {
  return RenderListFlags::REORDERED == (flags & RenderListFlags::REORDERED);
}

void RenderList::binarySearchReorder(const GraphicBatchHandler &handler) {
  auto &batch = this->batch;
  auto key    = handler.rendererKey;

  size_t pos = static_cast<size_t>(&handler - &batch[0]);
  auto it    = std::find(orderIndices.begin(), orderIndices.end(), pos);
  if (it != orderIndices.end()) {
    pos = static_cast<size_t>(std::distance(orderIndices.begin(), it));
  } else {
    pos = SIZE_MAX;
  }

  if (pos == SIZE_MAX) {
    return; // Handler not found in orderIndices
  }
  // Remove the index from orderIndices
  size_t index = orderIndices[pos];
  orderIndices.erase(
      orderIndices.begin() + static_cast<std::vector<size_t>::difference_type>(pos));

  // Find the new position using binary search
  auto newPos = std::lower_bound(
      orderIndices.begin(), orderIndices.end(), index, [&batch, &key](size_t a, size_t) {
        return batch[a].rendererKey < key;
      });

  // Insert the index at the new position
  orderIndices.insert(newPos, static_cast<unsigned int>(&handler - &batch[0]));
}

void RenderList::radixReorder() {
  // Implement radix sort based on the BatchKey fields
  // This is a placeholder for the actual radix sort implementation
  // For simplicity, we will use std::sort here, but in a real scenario,
  // you would implement a proper radix sort for better performance.
  auto &batch = this->batch;
  std::sort(orderIndices.begin(), orderIndices.end(), [&batch](size_t a, size_t b) {
    return batch[a].rendererKey < batch[b].rendererKey;
  });
}

void RenderList::reorder() {
  // Choose between binary search reorder or radix reorder based on the number of dirty
  // indices
  if (dirtyHandlers.size() < orderIndices.size() / 4) {
    for (auto &handler : dirtyHandlers) {
      binarySearchReorder(handler);
    }
  } else {
    radixReorder();
  }
  dirtyHandlers.clear();
  this->flags |= RenderListFlags::REORDERED;
  this->flags &= ~RenderListFlags::NEEDS_REORDER;
}

void RenderList::markDirty(GraphicBatchHandler &handler) {
  dirtyHandlers.push_back(handler);
  this->flags |= RenderListFlags::NEEDS_REORDER;
}

void RenderList::uploadOrderIndices() {
  this->flags &= ~RenderListFlags::REORDERED;
  indexIndirectionSSBO->bind();
  indexIndirectionSSBO->upload(
      this->orderIndices.data(), this->orderIndices.size() * sizeof(int), 0);
}

} // namespace RaeptorCogs::GAPI::Common