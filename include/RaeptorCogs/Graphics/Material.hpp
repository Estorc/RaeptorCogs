#include "RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp"
#include "RaeptorCogs/Graphics/GAPI/Common/Resources/Shader.hpp"

using namespace RaeptorCogs::GAPI::Common;
using namespace RaeptorCogs::GAPI;

namespace RaeptorCogs {

class Material {
  private:
    ObjectHandler<Shader> *shader = nullptr;
    void *data                    = nullptr;

  public:
    Material()  = default;
    ~Material() = default;

    void setShader(ObjectHandler<Shader> *newShader) {
      shader = newShader;
    }
    template <typename T>
    void setData(T newData) {
      if (this->shader->get() == nullptr) {
        std::cerr
            << "Shader must be initialized before setting material data." << std::endl;
        return;
      }
      if (this->shader->get()) data = static_cast<void *>(&newData);
    }
};

} // namespace RaeptorCogs