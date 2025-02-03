#pragma once

#include <Base/Types.hpp>
#include <Core/Application.hpp>
#include <Graphics/Cameras/ArcballCamera.hpp>
#include <Graphics/Handles.hpp>
#include <Graphics/OpenGL/OpenGLProgram.hpp>
#include <Graphics/OpenGL/OpenGLUniform.hpp>

#include <MTL/Matrices/Mat4x4.hpp>

#include "Game.hpp"

namespace Krys
{
  struct Uniforms final
  {
    Gfx::OpenGL::OpenGLUniform<Mat4> Transform;
    Gfx::OpenGL::OpenGLUniform<Mat4> View;
    Gfx::OpenGL::OpenGLUniform<Mat4> Projection;

    Uniforms() noexcept = default;

    Uniforms(Gfx::OpenGL::OpenGLProgram &program) noexcept
        : Transform("u_Transform", program.GetHandle(), program.GetNativeHandle()),
          View("u_View", program.GetHandle(), program.GetNativeHandle()),
          Projection("u_Projection", program.GetHandle(), program.GetNativeHandle())
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
    Gfx::MeshHandle _cubeMesh;
    Gfx::ProgramHandle _shader;
    Gfx::TextureHandle _texture;
    Gfx::MaterialHandle _material;
    Uniforms _uniforms;
    Gfx::ArcballCamera _camera;
  };
}