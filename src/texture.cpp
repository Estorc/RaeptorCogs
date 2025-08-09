#include <RaeptorLab/texture.hpp>
#include <RaeptorLab/bitOp.hpp>

#pragma region TextureAtlas

TextureAtlas::TextureAtlas() : TextureAtlas(glm::ivec2(COMMON_ATLAS_WIDTH, COMMON_ATLAS_HEIGHT)) {}
TextureAtlas::TextureAtlas(glm::ivec2 size) : textureID(0), flags(TextureAtlasFlags::NONE) {
    std::cout << "Creating TextureAtlas with size: " << size.x << "x" << size.y << std::endl;
    this->size = size;
    this->nodes.resize(this->size.x);
    stbrp_init_target(&this->ctx, this->size.x, this->size.y, this->nodes.data(), this->size.x);
}

void TextureAtlas::bind() const {
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

void TextureAtlas::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureAtlas::uploadTexture(GLuint x, GLuint y, GLuint width, GLuint height, const void *data) {
    if (this->textureID == 0) {
        glGenTextures(1, &this->textureID);
        glBindTexture(GL_TEXTURE_2D, this->textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4.0f);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->size.x, this->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    glBindTexture(GL_TEXTURE_2D, this->textureID);

    int innerW = width - ATLAS_PADDING * 2;
    int innerH = height - ATLAS_PADDING * 2;
    const unsigned char *pixels = (const unsigned char*)data;

    // --- Upload the center region ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ATLAS_PADDING, y + ATLAS_PADDING,
        innerW, innerH,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Left padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x, y + ATLAS_PADDING,
        ATLAS_PADDING, innerH,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Right padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ATLAS_PADDING + innerW, y + ATLAS_PADDING,
        ATLAS_PADDING, innerH,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + (innerW - 1)) * 4);

    // --- Top padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ATLAS_PADDING, y,
        innerW, ATLAS_PADDING,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Bottom padding ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, innerW);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ATLAS_PADDING, y + ATLAS_PADDING + innerH,
        innerW, ATLAS_PADDING,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + ((innerH - 1) * innerW + 0) * 4);

    // --- Top-left corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x, y,
        ATLAS_PADDING, ATLAS_PADDING,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + 0) * 4);

    // --- Top-right corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ATLAS_PADDING + innerW, y,
        ATLAS_PADDING, ATLAS_PADDING,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + (0 * innerW + (innerW - 1)) * 4);

    // --- Bottom-left corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x, y + ATLAS_PADDING + innerH,
        ATLAS_PADDING, ATLAS_PADDING,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + ((innerH - 1) * innerW + 0) * 4);

    // --- Bottom-right corner ---
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        x + ATLAS_PADDING + innerW, y + ATLAS_PADDING + innerH,
        ATLAS_PADDING, ATLAS_PADDING,
        GL_RGBA, GL_UNSIGNED_BYTE,
        pixels + ((innerH - 1) * innerW + (innerW - 1)) * 4);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // reset

    glGenerateMipmap(GL_TEXTURE_2D);
}

bool TextureAtlas::tryAddTexture(Texture &texture, const Image &img) {
    stbrp_rect r;
    r.w = img.width + ATLAS_PADDING * 2;
    r.h = img.height + ATLAS_PADDING * 2;
    r.id = 0;

    if (!stbrp_pack_rects(&this->ctx, &r, 1) || !r.was_packed) {
        std::cerr << "Failed to pack texture into atlas!" << std::endl;
        return false; // Packing failed
    }

    texture.setAtlas(this);
    texture.setRect(glm::vec4(r.x, r.y, r.w, r.h));

    this->textures.push_back(texture);
    return true;
}

void TextureAtlas::removeTexture(const Texture &texture) {
    /*auto it = std::remove(this->textures.begin(), this->textures.end(), texture);
    if (it != this->textures.end()) {
        this->textures.erase(it, this->textures.end());
    }*/
}

GLuint TextureAtlas::getID() const {
    return this->textureID;
}

int TextureAtlas::getWidth() const {
    return this->size.x;
}

int TextureAtlas::getHeight() const {
    return this->size.y;
}

#pragma endregion
#pragma region Texture

Texture::Texture(const Image &img) : atlas(nullptr) {
    std::cout << "Creating Texture from Image with size: " << img.width << "x" << img.height << std::endl;
    if (!img.data) {
        std::cerr << "Failed to load texture from image data." << std::endl;
        return;
    }
    this->opaque = img.isOpaque();
    TextureAtlas *atlas = textureAtlasManager.getAtlas();
    uint64_t allocatedSize = next_power_of_2_64(std::max(img.width + ATLAS_PADDING * 2, img.height + ATLAS_PADDING * 2));
    if (allocatedSize < COMMON_ATLAS_WIDTH) {
        allocatedSize = COMMON_ATLAS_WIDTH;
    }

    if (!atlas) atlas = new TextureAtlas(glm::ivec2(allocatedSize, allocatedSize));

    if (atlas->tryAddTexture(*this, img)) {
        atlas->uploadTexture(this->rect.x, this->rect.y, this->rect.z, this->rect.w, img.data.get());
        textureAtlasManager.addAtlas(*atlas);
    } else {
        atlas = new TextureAtlas(glm::ivec2(allocatedSize, allocatedSize));
        if (atlas->tryAddTexture(*this, img)) {
            atlas->uploadTexture(this->rect.x, this->rect.y, this->rect.z, this->rect.w, img.data.get());
            textureAtlasManager.addAtlas(*atlas);
        } else {
            std::cerr << "Failed to add texture to new atlas." << std::endl;
            this->atlas = nullptr;
        }
    }
}

Texture::Texture(const char *filepath) : Texture::Texture(load_image(filepath)) {}

void Texture::bind() const {
    if (this->atlas) {
        this->atlas->bind();
    } else {
        std::cerr << "Texture atlas is not set for this texture!" << std::endl;
    }
}

void Texture::setAtlas(TextureAtlas *atlas) {
    this->atlas = atlas;
}

void Texture::setUVRect(const glm::vec4 &uv) {
    this->uvRect = uv;
}

void Texture::setRect(const glm::vec4 &rect) {
    this->rect = rect;
    this->setUVRect(glm::vec4(
        (rect.x+ATLAS_PADDING) / this->atlas->getWidth(),
        (rect.y+ATLAS_PADDING) / this->atlas->getHeight(),
        (rect.z-ATLAS_PADDING*2) / this->atlas->getWidth(),
        (rect.w-ATLAS_PADDING*2) / this->atlas->getHeight()
    ));
}

glm::vec4 Texture::getRect() {
    return this->rect;
}

glm::vec4 Texture::getUVRect() {
    return this->uvRect;
}

TextureAtlas *Texture::getAtlas() const {
    return this->atlas;
}

GLuint Texture::getID() const {
    return this->atlas->getID();
}

bool Texture::isOpaque() const {
    return this->opaque;
}

#pragma endregion
#pragma region TextureAtlasManager

TextureAtlasManager::TextureAtlasManager() = default;

void TextureAtlasManager::addAtlas(const TextureAtlas &atlas) {
    this->atlases.push(atlas);
}

void TextureAtlasManager::removeAtlas(const TextureAtlas &atlas) {
    // This is a placeholder for actual removal logic
    // You might need to implement a way to find and remove the specific atlas
}

TextureAtlas* TextureAtlasManager::getAtlas() {
    if (this->atlases.empty()) {
        return nullptr; // No atlases available
    }
    return &this->atlases.top();
}

TextureAtlasManager textureAtlasManager = TextureAtlasManager();

#pragma endregion