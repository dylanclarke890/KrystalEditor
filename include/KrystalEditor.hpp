#include <Core/Application.hpp>

#include "Game.hpp"

namespace Krys
{
  class KrystalEditor : public Application
  {
  public:
    KrystalEditor(Unique<ApplicationContext> context, const ApplicationSettings &settings) noexcept;
    ~KrystalEditor() noexcept override = default;

    void Update(float dt) noexcept override;
    void Render() noexcept override;

  private:
    Unique<Game> _game;
  };
}