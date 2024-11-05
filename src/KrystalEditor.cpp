#include "KrystalEditor.h"

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context, uint32 width, uint32 height) noexcept
      : Application(std::move(context), width, height)
  {
  }

  void KrystalEditor::Update(float dt) noexcept
  {
  }
}