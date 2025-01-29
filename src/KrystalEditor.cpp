#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Events/Input/MouseMoveEvent.hpp>
#include <Core/Events/Input/ScrollWheelEvent.hpp>
#include <Core/Events/QuitEvent.hpp>
#include <Core/Logger.hpp>
#include <Core/Platform.hpp>
#include <Graphics/Colours.hpp>
#include <Graphics/OpenGL/OpenGLTexture.hpp>
#include <IO/Images.hpp>
#include <IO/IO.hpp>
#include <MTL/Common/Convert.hpp>
#include <MTL/Matrices/Ext/Transformations.hpp>
#include <MTL/Vectors/Ext/Format.hpp>

#include "Pong.hpp"

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context) noexcept
      : Application(std::move(context)), _game(CreateUnique<Pong>(_context.get())), _cubeMesh(), _shader(),
        _texture(), _camera(Gfx::CameraType::Perspective, 1'920, 1'080, 100, Vec3(0.0f), 10.0f)
  {
  }

  void KrystalEditor::OnInit() noexcept
  {
    _context->GetWindowManager()->GetCurrentWindow()->ShowCursor(true);
    // TODO: this doesn't seem to work
    _context->GetWindowManager()->GetCurrentWindow()->LockCursor(true);

    BindEvents();

    auto graphicsContext = _context->GetGraphicsContext();
    graphicsContext->SetClearColour(Gfx::Colours::Black);

    {
      using namespace Gfx;
      _cubeMesh = _context->GetMeshManager()->CreateCube(Colours::White);
    }

    auto vertexShader =
      graphicsContext->CreateShader(Gfx::ShaderStage::Vertex, IO::ReadFileText("shaders/triangle.vert"));
    auto fragmentShader =
      graphicsContext->CreateShader(Gfx::ShaderStage::Fragment, IO::ReadFileText("shaders/triangle.frag"));

    _shader = graphicsContext->CreateProgram(vertexShader, fragmentShader);
    _texture = _context->GetTextureManager()->LoadTexture("textures/wood-wall.jpg");
    _material = _context->GetMaterialManager()->CreatePhongMaterial(_shader, _texture);

    _uniforms = Uniforms(*static_cast<Gfx::OpenGL::OpenGLProgram *>(graphicsContext->GetProgram(_shader)));
  }

  void KrystalEditor::OnShutdown() noexcept
  {
  }

  void KrystalEditor::OnRender() noexcept
  {
    using namespace Gfx;

    auto renderer = _context->GetRenderer();

    {
      RenderCommand command;
      command.Program = _shader;
      command.Mesh = _cubeMesh;
      command.Material = _material;
      renderer->Submit(command);
    }

    auto ctx = _context->GetGraphicsContext();
    ctx->Clear(ClearBuffer::Colour | ClearBuffer::Depth);
    renderer->Execute();
  }

  void KrystalEditor::OnUpdate(float) noexcept
  {
    auto *input = _context->GetInputManager();

    if (input->GetMouse().IsButtonPressed(MouseButton::LEFT))
    {
      auto &mouse = input->GetMouse();
      _camera.OnMouseDragStart({mouse.GetClientX(), mouse.GetClientY()});
    }
    else if (input->GetMouse().IsButtonHeld(MouseButton::LEFT))
    {
      auto &mouse = input->GetMouse();
      _camera.OnMouseDrag({mouse.GetClientX(), mouse.GetClientY()});
    }

    Mat4 trans = Mat4(1.0f);
    // trans = MTL::Rotate(trans, static_cast<float>(Platform::GetTime()), Vec3(0.0, 0.0, 1.0));
    // trans = MTL::Scale(trans, Vec3(0.5, 0.5, 0.5));

    _uniforms.Transform.SetValue(trans);
    _uniforms.View.SetValue(_camera.GetView());
    _uniforms.Projection.SetValue(_camera.GetProjection());
  }

  void KrystalEditor::OnFixedUpdate(float) noexcept
  {
  }

  void KrystalEditor::BindEvents() noexcept
  {
    auto eventManager = _context->GetEventManager();
    eventManager->RegisterHandler<KeyboardEvent>(
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

    eventManager->RegisterHandler<QuitEvent>(
      [&](const QuitEvent &)
      {
        _running = false;
        return true;
      });

    eventManager->RegisterHandler<ScrollWheelEvent>(
      [&](const ScrollWheelEvent &event)
      {
        _camera.Zoom(event.Delta());
        return false;
      });
  }
}