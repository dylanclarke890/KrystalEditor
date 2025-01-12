#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <IO/Images/ImageLoader.hpp>

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

    IO::ImageLoader<> loader("textures/brickwall.jpg");
    auto result = loader.Load();

    KRYS_ASSERT(result.has_value(), "Error: %s", result.error().c_str());

    _running = false;
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}