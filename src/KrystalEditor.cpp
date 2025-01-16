#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Logger.hpp>
#include <IO/Images/ImageLoader.hpp>

#include "Pong.hpp"

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context,
                               const ApplicationSettings &settings) noexcept
      : Application(std::move(context), settings), _game(CreateUnique<Pong>(_context.get()))
  {
    _context->GetEventManager()->RegisterHandler<KeyboardEvent>(
      [&](const KeyboardEvent &event)
      {
        Logger::Info("Key pressed: {0}", event.GetKey());

        if (event.GetKey() == Key::ESCAPE)
        {
          _running = false;
          return true;
        }

        return false;
      });

    IO::ImageLoader<> loader("textures/brickwall.jpg");
    auto result = loader.Load();
    KRYS_ASSERT(result.has_value(), "Error: {0}", result.error());
  }

  void KrystalEditor::Update(float) noexcept
  {
  }

  void KrystalEditor::Render() noexcept
  {
  }
}