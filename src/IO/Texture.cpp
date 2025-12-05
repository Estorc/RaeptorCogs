#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Buffer.hpp>
#include <RaeptorCogs/BitOp.hpp>
#include <algorithm>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
namespace RaeptorCogs {

#pragma region TextureAtlas



TextureAtlas::TextureAtlas() : TextureAtlas(glm::ivec2(COMMON_ATLAS_SIZE, COMMON_ATLAS_SIZE)) {}
TextureAtlas::TextureAtlas(glm::ivec2 size, GLenum minFilter, GLenum magFilter) : flags(TextureAtlasFlags::NONE), minFilter(minFilter), magFilter(magFilter) {
    #ifndef NDEBUG
    std::cout << "Creating TextureAtlas with size: " << size.x << "x" << size.y << std::endl;
    #endif
    this->size = size;
    this->freeSpace = size.x * size.y;
    this->nodes.resize(static_cast<size_t>(this->size.x));
    stbrp_init_target(&this->ctx, this->size.x, this->size.y, this->nodes.data(), this->size.x);
}

void TextureAtlas::bind() {
    this->glTexture->bind();
}

void TextureAtlas::unbind() const {
    this->glTexture->unbind();
}

void TextureAtlas::uploadTexture(GLint x, GLint y, GLint width, GLint height, const void *data, bool newAtlas) {
    if (newAtlas) {
        this->glTexture->build(this->size.x, this->size.y, nullptr, this->minFilter, this->magFilter);
    }

    this->glTexture->bind();

    GLint ipadding = static_cast<GLint>(ATLAS_PADDING);

    GLint innerW = width - ipadding * 2;
    GLint innerH = height - ipadding * 2;
    const unsigned char *pixels = (const unsigned char*)data;

    // --- Upload the center region ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ipadding, y + ipadding,
        innerW, innerH,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Left padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x, y + ipadding,
        ipadding, innerH,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Right padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ipadding + innerW, y + ipadding,
        ipadding, innerH,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + (innerW - 1)) * 4);

    // --- Top padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ipadding, y,
        innerW, ipadding,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Bottom padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ipadding, y + ipadding + innerH,
        innerW, ipadding,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + ((innerH - 1) * innerW + 0) * 4);

    // --- Top-left corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x, y,
        ipadding, ipadding,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Top-right corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ipadding + innerW, y,
        ipadding, ipadding,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + (innerW - 1)) * 4);

    // --- Bottom-left corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x, y + ipadding + innerH,
        ipadding, ipadding,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + ((innerH - 1) * innerW + 0) * 4);

    // --- Bottom-right corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ipadding + innerW, y + ipadding + innerH,
        ipadding, ipadding,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + ((innerH - 1) * innerW + (innerW - 1)) * 4);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // reset

    glGenerateMipmap(GL_TEXTURE_2D);
}

bool TextureAtlas::tryAddTexture(TextureBase *texture, int width, int height) {
    stbrp_rect r{};
    r.w = width + static_cast<int>(ATLAS_PADDING * 2);
    r.h = height + static_cast<int>(ATLAS_PADDING * 2);
    r.id = 0;

    if (!stbrp_pack_rects(&this->ctx, &r, 1) || !r.was_packed) {
        std::cerr << "Failed to pack texture into atlas!" << std::endl;
        return false; // Packing failed
    }

    this->freeSpace -= (r.w * r.h);
    texture->setRect(glm::vec4(r.x, r.y, r.w, r.h));
    texture->setUVRect(glm::vec4(
        (float)(r.x + static_cast<int>(ATLAS_PADDING)) / (float)this->size.x,
        (float)(r.y + static_cast<int>(ATLAS_PADDING)) / (float)this->size.y,
        (float)(width) / (float)this->size.x,
        (float)(height) / (float)this->size.y
    ));

    this->textures.push_back(texture);
    return true;
}

void TextureAtlas::removeTexture(const TextureBase &texture) {
    auto it = std::find(this->textures.begin(), this->textures.end(), &texture);
    if (it != this->textures.end()) {
        this->textures.erase(it);
    }
    this->freeSpace = this->size.x * this->size.y;
    stbrp_init_target(&this->ctx, this->size.x, this->size.y, this->nodes.data(), this->size.x);
    std::vector<stbrp_rect> rects;
    rects.reserve(this->textures.size());
    for (auto tex : this->textures) {
        stbrp_rect *r = &rects.emplace_back();
        glm::vec4 texRect = tex->getRect();
        r->w = static_cast<int>(texRect.z);
        r->h = static_cast<int>(texRect.w);
        r->id = 0;
        this->freeSpace -= (r->w * r->h);
        if (!stbrp_pack_rects(&this->ctx, r, 1) || !r->was_packed) {
            std::cerr << "Failed to repack texture into atlas!" << std::endl;
            return;
        }
    }
    GAPI::ObjectHandler<GAPI::Common::FBO> fbo;
    fbo->bind();
    GAPI::ObjectHandler<GAPI::Common::TextureData> newTexture;
    newTexture->build(this->size.x, this->size.y, nullptr, minFilter, magFilter);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getID());
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->getID(), 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getID());
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, newTexture->getID(), 0);
    GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawBuffers);
    for (size_t i = 0; i < this->textures.size(); ++i) {
        glm::vec4 oldRect = this->textures[i]->getRect();
        glm::vec4 newRect = glm::vec4(rects[i].x, rects[i].y, rects[i].w, rects[i].h);
        this->textures[i]->setRect(newRect);
        this->textures[i]->setUVRect(glm::vec4(
            (float)(newRect.x + ATLAS_PADDING) / (float)this->size.x,
            (float)(newRect.y + ATLAS_PADDING) / (float)this->size.y,
            (float)(newRect.z - ATLAS_PADDING * 2) / (float)this->size.x,
            (float)(newRect.w - ATLAS_PADDING * 2) / (float)this->size.y
        ));
        glBlitFramebuffer(
            static_cast<GLint>(oldRect.x), static_cast<GLint>(oldRect.y), static_cast<GLint>(oldRect.x + oldRect.z), static_cast<GLint>(oldRect.y + oldRect.w),
            static_cast<GLint>(newRect.x), static_cast<GLint>(newRect.y), static_cast<GLint>(newRect.x + newRect.z), static_cast<GLint>(newRect.y + newRect.w),
            GL_COLOR_BUFFER_BIT, GL_NEAREST
        );
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getID());
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexture->getID(), 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getID());
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->getID(), 0);
    glDrawBuffers(2, drawBuffers);
    glBlitFramebuffer(
        0, 0, this->size.x, this->size.y,
        0, 0, this->size.x, this->size.y,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );
    fbo->unbind();
    this->bind();
    glGenerateMipmap(GL_TEXTURE_2D);
    if ((this->flags & TextureAtlasFlags::NEEDS_REBUILD) == TextureAtlasFlags::NONE) {
        this->flags |= TextureAtlasFlags::NEEDS_REBUILD;
        RaeptorCogs::MainWorker().addJob([this]() {
            this->flags &= ~TextureAtlasFlags::NEEDS_REBUILD;
        }, 1);
    }
}

GLuint TextureAtlas::getID() const {
    return this->glTexture->getID();
}

int TextureAtlas::getWidth() const {
    return this->size.x;
}

int TextureAtlas::getHeight() const {
    return this->size.y;
}

int TextureAtlas::getFreeSpace() const {
    return this->freeSpace;
}

bool TextureAtlas::needsRebuild() const {
    return (this->flags & TextureAtlasFlags::NEEDS_REBUILD) != TextureAtlasFlags::NONE;
}

#pragma endregion
#pragma region Texture


TextureBase::~TextureBase() {
    if (auto locked = this->atlas.lock()) {
        locked->removeTexture(*this);
    }
}

void TextureBase::upload(const Image &img) {
    if (!img.data) {
        std::cerr << "Failed to load texture from image data." << std::endl;
        return;
    }
    std::shared_ptr<TextureAtlas> atlas = RaeptorCogs::TextureAtlasManager().getAtlas(std::make_tuple(GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR));
    uint64_t allocatedSize = NextPowerOf2(std::max(img.width + ATLAS_PADDING * 2, img.height + ATLAS_PADDING * 2));
    if (allocatedSize < COMMON_ATLAS_SIZE) {
        allocatedSize = COMMON_ATLAS_SIZE;
    }

    bool needsNewAtlas = !atlas;
    bool textureAdded = false;
    if (!needsNewAtlas) {
        needsNewAtlas = !atlas->tryAddTexture(this, static_cast<int>(img.width), static_cast<int>(img.height));
        textureAdded = !needsNewAtlas;
    }
    if (needsNewAtlas) atlas = std::make_shared<TextureAtlas>(glm::ivec2(allocatedSize, allocatedSize), GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
    if (textureAdded || atlas->tryAddTexture(this, static_cast<int>(img.width), static_cast<int>(img.height))) {
        atlas->uploadTexture(static_cast<GLint>(this->rect.x), static_cast<GLint>(this->rect.y), static_cast<GLint>(this->rect.z), static_cast<GLint>(this->rect.w), img.data.get(), needsNewAtlas);
        if (needsNewAtlas) RaeptorCogs::TextureAtlasManager().addAtlas(atlas);
        this->setAtlas(atlas);
    } else {
        std::cerr << "Failed to add texture to new atlas." << std::endl;
        this->atlas.reset();
    }
}

std::shared_ptr<TextureBase> TextureBase::create(const FileData &fileData, TextureOptions options) {
    auto texture = std::shared_ptr<TextureBase>(new TextureBase());
    RaeptorCogs::ResourceWorker().addJob([self = texture, fileData, options]() {
        auto img = std::make_shared<Image>(LoadImageFromMemory(fileData, options.s_width, options.s_height));
        if (!img->data) {
            std::cerr << "Failed to create texture from image: No data." << std::endl;
            return;
        }
        self->opaque = img->isOpaque();
        RaeptorCogs::MainWorker().addJob([self, img]() {
            if (self.use_count() <= 2) return; // If no one else is using this texture, skip uploading
            self->upload(*img);
            if (self->onLoad_) self->onLoad_();
        }, options.priority);
    }, options.priority);
    return texture;
}
std::shared_ptr<TextureBase> TextureBase::create(const std::filesystem::path& filepath, TextureOptions options) {
    auto texture = std::shared_ptr<TextureBase>(new TextureBase());
    if (filepath.empty()) {
        std::cerr << "Failed to create texture: Filepath is empty." << std::endl;
        return texture;
    }
    RaeptorCogs::ResourceWorker().addJob([self = texture, str = filepath.string(), options]() {
        auto img = std::make_shared<Image>(LoadImageFromFile(str.c_str(), options.s_width, options.s_height));
        if (!img->data) {
            std::cerr << "Failed to create texture from image: No data." << std::endl;
            return;
        }
        RaeptorCogs::MainWorker().addJob([self, img]() {
            if (self.use_count() <= 2) return; // If no one else is using this texture, skip uploading
            self->opaque = img->isOpaque();
            self->upload(*img);
            if (self->onLoad_) self->onLoad_();
        }, options.priority);
    }, options.priority);
    return texture;
}
std::shared_ptr<TextureBase> TextureBase::create(unsigned int width, unsigned int height, TextureOptions options) {
    auto texture = std::shared_ptr<TextureBase>(new TextureBase());
    RaeptorCogs::ResourceWorker().addJob([self = texture, width, height, options]() {
        auto img = std::make_shared<Image>(CreateImage(width, height));
        if (!img->data) {
            std::cerr << "Failed to create texture from image: No data." << std::endl;
            return;
        }
        self->opaque = false;
        RaeptorCogs::MainWorker().addJob([self, img]() {
            if (self.use_count() <= 2) return; // If no one else is using this texture, skip uploading
            self->upload(*img);
            if (self->onLoad_) self->onLoad_();
        }, options.priority);
    }, options.priority);
    return texture;
}
std::shared_ptr<TextureBase> TextureBase::create(const Image &img) {
    auto texture = std::shared_ptr<TextureBase>(new TextureBase());
    if (!img.data) {
        std::cerr << "Failed to create texture from image: No data." << std::endl;
        return texture;
    }
    texture->opaque = img.isOpaque();
    texture->upload(img);
    if (texture->onLoad_) texture->onLoad_();
    return texture;
}

void TextureBase::bind() const {
    if (this->isLoaded()) {
        if (auto locked = this->atlas.lock()) {
            locked->bind();
        } else {
            throw std::runtime_error("TextureBase::bind() failed: Atlas is expired.");
        }
    }
}

void TextureBase::setAtlas(std::shared_ptr<TextureAtlas> atlas) {
    this->atlas = atlas;
}

void TextureBase::setUVRect(const glm::vec4 &uv) {
    this->uvRect = uv;
}

void TextureBase::setRect(const glm::vec4 &rect) {
    this->rect = rect;
}

float TextureBase::getX() const {
    return this->rect.x;
}

float TextureBase::getY() const {
    return this->rect.y;
}

float TextureBase::getWidth() const {
    return this->rect.z;
}

float TextureBase::getHeight() const {
    return this->rect.w;
}

glm::vec4 TextureBase::getRect() {
    return this->rect;
}

glm::vec4 TextureBase::getUVRect() {
    return this->uvRect;
}

TextureAtlas *TextureBase::getAtlas() const {
    return this->atlas.lock().get();
}

GLuint TextureBase::getID() const {
    return this->atlas.lock() ? this->atlas.lock()->getID() : 0;
}

bool TextureBase::isOpaque() const {
    return this->opaque;
}

bool TextureBase::isLoaded() const {
    return this->atlas.lock() != nullptr;
}

bool TextureBase::needsRebuild() const {
    auto locked = this->atlas.lock();
    return locked ? locked->needsRebuild() : false;
}

#pragma endregion

}

namespace RaeptorCogs::Singletons {

#pragma region TextureAtlasManager

TextureAtlasManager::~TextureAtlasManager() {
    /*while (!this->atlases.empty()) {
        TextureAtlas *atlas = this->atlases.top();
        this->atlases.pop();
        delete atlas;
    }*/
}

void TextureAtlasManager::addAtlas(std::shared_ptr<TextureAtlas> atlas) {
    TextureAtlasTypeKey key = atlas->getTypeKey();
    this->atlases[key].push_back(atlas);
    this->sort(key);
    #ifndef NDEBUG
    std::cout << "Added atlas. Total atlases of this type: " << this->atlases[key].size() << std::endl;
    #endif
}

void TextureAtlasManager::sort(TextureAtlasTypeKey key) {
    auto it = this->atlases.find(key);
    if (it == this->atlases.end() || it->second.size() < 2) {
        return; // No need to sort
    }
    std::sort(it->second.begin(), it->second.end(), [](const std::shared_ptr<TextureAtlas> &a, const std::shared_ptr<TextureAtlas> &b) {
        return (a->getFreeSpace()) < (b->getFreeSpace());
    });
}

void TextureAtlasManager::removeAtlas(const TextureAtlas *atlas) {
    (void) atlas;
    // Placeholder for future implementation if needed
}

std::shared_ptr<TextureAtlas> TextureAtlasManager::getAtlas(TextureAtlasTypeKey key) {
    auto it = this->atlases.find(key);
    if (it != this->atlases.end() && !it->second.empty()) {
        return it->second.back(); // Return the last atlas of the requested type
    }
    return nullptr;
}

#pragma endregion

}