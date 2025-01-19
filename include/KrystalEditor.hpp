#include <Core/Application.hpp>

#include "Game.hpp"

namespace Krys
{
  class KrystalEditor : public Application
  {
  public:
    KrystalEditor(Unique<ApplicationContext> context) noexcept;
    ~KrystalEditor() noexcept override = default;

    void Update(float dt) noexcept override;
    void FixedUpdate(float dt) noexcept override;
    void Render() noexcept override;

  private:
    void BindEvents() noexcept;

    Unique<Game> _game;
  };
}