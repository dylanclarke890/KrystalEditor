#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Events/QuitEvent.hpp>
#include <Core/Logger.hpp>
#include <Core/Platform.hpp>
#include <Graphics/Colours.hpp>
#include <IO/Images.hpp>
#include <IO/IO.hpp>

#include "Pong.hpp"

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context) noexcept
      : Application(std::move(context)), _game(CreateUnique<Pong>(_context.get()))
  {
  }

  void KrystalEditor::OnInit() noexcept
  {
    BindEvents();

    auto graphicsContext = _context->GetGraphicsContext();
    graphicsContext->SetClearColour(Gfx::Colours::Coral);

    {
      using namespace Gfx;
      _triangleMesh =
        _context->GetMeshManager()->CreateMesh({VertexData {Vec3 {-0.5f, -0.5f, 0.0f}, Colours::Cyan},
                                                VertexData {Vec3 {0.5f, -0.5f, 0.0f}, Colours::Beige},
                                                VertexData {Vec3 {0.0f, 0.5f, 0.0f}, Colours::Red}},
                                               {0, 1, 2});
    }

    auto vertexShader = graphicsContext->CreateShader(
      Gfx::ShaderDescriptor {Gfx::ShaderStage::Vertex, IO::ReadFileText("shaders/triangle.vert")});
    auto fragmentShader = graphicsContext->CreateShader(
      Gfx::ShaderDescriptor {Gfx::ShaderStage::Fragment, IO::ReadFileText("shaders/triangle.frag")});

    _triangleShader = graphicsContext->CreatePipeline();
    auto &pipeline = graphicsContext->GetPipeline(_triangleShader);

    pipeline.AddShader(vertexShader);
    pipeline.AddShader(fragmentShader);
    pipeline.Link();

    KRYS_ASSERT(pipeline.IsValid(), "Pipeline is not valid");

    // _timeUniform = Gfx::OpenGL::OpenGLUniform<float32>(_triangleShader, "u_Time");

    auto result = IO::LoadImage("textures/tiles.jpg");
    KRYS_ASSERT(result, "Failed to load image: {0}", result.error());
  }

  void KrystalEditor::OnShutdown() noexcept
  {
  }

  void KrystalEditor::OnRender() noexcept
  {
    using namespace Gfx;

    auto renderer = _context->GetRenderer();

    // {
    //   auto time = Platform::GetTime();
    //   _timeUniform.SetValue(time);
    // }

    {
      RenderCommand command;
      command.Pipeline = _triangleShader;
      command.Mesh = _triangleMesh;
      renderer->Submit(command);
    }

    auto ctx = _context->GetGraphicsContext();
    ctx->Clear();
    renderer->Execute(ctx);
  }

  void KrystalEditor::OnUpdate(float) noexcept
  {
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