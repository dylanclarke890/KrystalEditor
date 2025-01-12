#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Scripting/Syrk.hpp>

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

    Scripting::Syrk syrk;
    syrk.RunFile("scripts/test.syrk");

    _running = false;
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}