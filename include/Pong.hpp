#pragma once

#include "Game.hpp"

namespace Krys
{
  class Pong final : public Game
  {
  public:
    explicit Pong(Game::context_t context) noexcept : Game(context)
    {
    }

    void Init() noexcept override
    {
    }

    void Update(float) noexcept override
    {
    }
  };
}