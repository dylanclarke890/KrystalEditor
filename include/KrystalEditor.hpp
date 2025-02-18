#pragma once

#include <Base/Types.hpp>
#include <Core/Application.hpp>
#include <Graphics/Cameras/ArcballCamera.hpp>
#include <Graphics/Handles.hpp>
#include <Graphics/OpenGL/OpenGLProgram.hpp>
#include <Graphics/OpenGL/OpenGLUniform.hpp>
#include <Graphics/RenderPipeline.hpp>
#include <MTL/Matrices/Mat4x4.hpp>

#include "Game.hpp"

namespace Krys
{
  class KrystalEditor : public Application
  {
  public:
    KrystalEditor(Unique<ApplicationContext> context) noexcept;
    ~KrystalEditor() noexcept override = default;

    void OnInit() noexcept override;
    void OnUpdate(float dt) noexcept override;
    void OnFixedUpdate(float dt) noexcept override;
    void OnRender() noexcept override;
    void OnShutdown() noexcept override;

  private:
    void BindEvents() noexcept;

    void SetupScene() noexcept;

    void SetupPipeline() noexcept;

    Unique<Game> _game;
    Gfx::ArcballCamera _camera;
    Gfx::RenderPipeline _pipeline;
  };
}