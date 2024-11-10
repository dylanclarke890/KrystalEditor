#include "KrystalEditor.hpp"
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Debug/Macros.hpp>

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context, uint32 width, uint32 height) noexcept
      : Application(std::move(context), width, height)
  {
    _context->GetEventManager()->RegisterHandler<KeyboardEvent>(
      [](const KeyboardEvent &event)
      {
        KRYS_LOG("key pressed %d", event.GetKey());
        return false;
      });
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}