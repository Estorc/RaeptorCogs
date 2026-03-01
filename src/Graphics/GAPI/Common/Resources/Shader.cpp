#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Shader.hpp>
#include <iostream>

namespace RaeptorCogs::GAPI::Common {

struct rgsl_shader_blob {
    const char *name;
    uint8_t stage;
    int version;
    const char *profile;
};

const void *FindEmbeddedShader(
    const void *rgsl_shaders, size_t shader_size, size_t shader_count,
    const std::string &name, ShaderStage stage,
    const std::vector<std::pair<ShaderStage, size_t>> &stages_map,
    const std::string &profile) {
  for (size_t i = 0; i < shader_count; i++) {
    const rgsl_shader_blob *shader = reinterpret_cast<const rgsl_shader_blob *>(
        static_cast<const char *>(rgsl_shaders) + i * shader_size);
    size_t mapped_stage = UINT8_MAX;
    for (const auto &pair : stages_map) {
      if (pair.first == stage) {
        mapped_stage = pair.second;
        break;
      }
    }
    if (mapped_stage == UINT8_MAX) {
      std::cerr << "Error: Unsupported shader stage requested." << std::endl;
      return nullptr;
    }
    if (name == shader->name && mapped_stage == shader->stage &&
        profile == shader->profile) {
      return shader;
    }
  }
  std::cerr
      << "Warning: Shader not found - Name: " << name << ", Stage: "
      << static_cast<size_t>(stage) << ", Profile: " << profile << std::endl;
  return nullptr;
}

} // namespace RaeptorCogs::GAPI::Common