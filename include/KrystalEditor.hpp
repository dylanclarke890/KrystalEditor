#include <Core/Application.hpp>

namespace Krys
{
  class KrystalEditor : public Application
  {
  public:
    KrystalEditor(Unique<ApplicationContext> context, uint32 width, uint32 height) noexcept;
    void Update(float dt) noexcept override;
  };
}