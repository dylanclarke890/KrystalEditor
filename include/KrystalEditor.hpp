#pragma once

#include <Base/Types.hpp>
#include <Core/Application.hpp>
#include <Graphics/ArcballCamera.hpp>
#include <Graphics/Handles.hpp>
#include <Graphics/OpenGL/OpenGLUniform.hpp>
#include <MTL/Matrices/Mat4x4.hpp>

#include "Game.hpp"

namespace Krys
{
  struct Uniforms final
  {
    Gfx::OpenGL::OpenGLUniform<uint64> Texture;
    Gfx::OpenGL::OpenGLUniform<Mat4> Transform;
    Gfx::OpenGL::OpenGLUniform<Mat4> View;
    Gfx::OpenGL::OpenGLUniform<Mat4> Projection;

    Uniforms() noexcept = default;

    Uniforms(Gfx::PipelineHandle shader) noexcept
        : Texture(shader, "u_Texture"), Transform(shader, "u_Transform"), View(shader, "u_View"),
          Projection(shader, "u_Projection")
    {
    }
  };

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
    Uniforms _uniforms;
    Gfx::ArcballCamera _camera;
  };
}