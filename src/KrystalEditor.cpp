#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Events/QuitEvent.hpp>
#include <Core/Logger.hpp>
#include <Core/Platform.hpp>
#include <Graphics/Colours.hpp>
#include <Graphics/OpenGL/OpenGLTexture.hpp>
#include <IO/Images.hpp>
#include <IO/IO.hpp>
#include <MTL/Common/Convert.hpp>
#include <MTL/Matrices/Ext/Transformations.hpp>

#include "Pong.hpp"

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context) noexcept
      : Application(std::move(context)), _game(CreateUnique<Pong>(_context.get())), _cubeMesh(), _shader(),
        _texture(), _camera(Gfx::CameraType::Perspective, 1'920, 1'080, 100)
  {
  }

  void KrystalEditor::OnInit() noexcept
  {
    BindEvents();

    auto graphicsContext = _context->GetGraphicsContext();
    graphicsContext->SetClearColour(Gfx::Colours::Black);

    {
      using namespace Gfx;
      _cubeMesh = _context->GetMeshManager()->CreateCube(Colours::White);
    }

    auto vertexShader = graphicsContext->CreateShader(
      Gfx::ShaderDescriptor {Gfx::ShaderStage::Vertex, IO::ReadFileText("shaders/triangle.vert")});
    auto fragmentShader = graphicsContext->CreateShader(
      Gfx::ShaderDescriptor {Gfx::ShaderStage::Fragment, IO::ReadFileText("shaders/triangle.frag")});

    _shader = graphicsContext->CreatePipeline();

    auto &pipeline = graphicsContext->GetPipeline(_shader);

    pipeline.AddShader(vertexShader);
    pipeline.AddShader(fragmentShader);
    pipeline.Link();

    KRYS_ASSERT(pipeline.IsValid(), "Pipeline is not valid");
    _uniforms = Uniforms(_shader);

    _texture = _context->GetTextureManager()->LoadTexture("textures/wood-wall.jpg");

    auto &glTexture =
      static_cast<Gfx::OpenGL::OpenGLTexture &>(_context->GetTextureManager()->GetTexture(_texture));
    _uniforms.Texture.SetValue(glTexture.GetNativeBindlessHandle());
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
      command.Pipeline = _shader;
      command.Mesh = _cubeMesh;
      renderer->Submit(command);
    }

    auto ctx = _context->GetGraphicsContext();
    ctx->Clear();
    renderer->Execute(ctx);
  }

  void KrystalEditor::OnUpdate(float) noexcept
  {
    auto *input = _context->GetInputManager();
    Logger::Info("Mouse delta: ({0}, {1})", input->GetMouse().DeltaX(), input->GetMouse().DeltaY());

    if (input->GetMouse().IsButtonHeld(MouseButton::LEFT))
    {
      auto &mouse = input->GetMouse();
      _camera.OnMouseDrag(mouse.DeltaX(), mouse.DeltaY());
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
  }
}