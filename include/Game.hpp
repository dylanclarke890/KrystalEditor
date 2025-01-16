#pragma once

#include <Base/Pointers.hpp>
#include <Core/Application.hpp>

namespace Krys
{
  class Game
  {
  protected:
    using context_t = Krys::Ptr<Krys::ApplicationContext>;

  public:
    Game() noexcept = delete;
    virtual ~Game() noexcept = default;

    explicit Game(context_t context) noexcept : _context(context)
    {
    }

    virtual void Init() noexcept = 0;
    virtual void Update(float) noexcept = 0;

  private:
    context_t _context;
  };
}