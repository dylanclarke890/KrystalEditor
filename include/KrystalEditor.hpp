#pragma once

#include <Base/Types.hpp>
#include <Core/Application.hpp>
#include <Graphics/Handles.hpp>
#include <Graphics/OpenGL/OpenGLUniform.hpp>

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

    Unique<Game> _game;
    Gfx::MeshHandle _triangleMesh;
    Gfx::PipelineHandle _triangleShader;
    Gfx::TextureHandle _texture;
    Gfx::OpenGL::OpenGLUniform<uint64> _textureUniform;
  };
}