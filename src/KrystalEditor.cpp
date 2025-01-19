#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Logger.hpp>
#include <Graphics/Colors.hpp>
#include <IO/IO.hpp>

#include "Pong.hpp"

namespace Krys
{
  Unique<Gfx::Mesh> triangleMesh;
  Gfx::PipelineHandle triangleShader;

  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context) noexcept
      : Application(std::move(context)), _game(CreateUnique<Pong>(_context.get()))
  {
    BindEvents();

    auto graphicsContext = _context->GetGraphicsContext();
    graphicsContext->SetClearColor(Colors::Olive);

    triangleMesh = _context->GetMeshManager()->CreateMesh(
      {{Vec3 {-0.5f, -0.5f, 0.0f}}, {Vec3 {0.5f, -0.5f, 0.0f}}, {Vec3 {0.0f, 0.5f, 0.0f}}}, {0, 1, 2});

    auto vertexShader = graphicsContext->CreateShader(
      Gfx::ShaderDescription {Gfx::ShaderStage::Vertex, IO::ReadFileText("shaders/triangle.vert")});
    auto fragmentShader = graphicsContext->CreateShader(
      Gfx::ShaderDescription {Gfx::ShaderStage::Fragment, IO::ReadFileText("shaders/triangle.frag")});

    triangleShader = graphicsContext->CreatePipeline();
    auto &pipeline = graphicsContext->GetPipeline(triangleShader);

    pipeline.AddShader(vertexShader);
    pipeline.AddShader(fragmentShader);
    pipeline.Link();

    KRYS_ASSERT(pipeline.IsValid(), "Pipeline is not valid");
  }

  void KrystalEditor::Update(float) noexcept
  {
  }

  void KrystalEditor::FixedUpdate(float) noexcept
  {
  }

  void KrystalEditor::Render() noexcept
  {
    using namespace Gfx;

    auto renderer = _context->GetRenderer();

    {
      RenderCommand command;
      command.Pipeline = triangleShader;
      command.Mesh = triangleMesh.get();
      renderer->Submit(command);
    }

    auto ctx = _context->GetGraphicsContext();
    ctx->Clear();
    renderer->Execute(ctx);
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
  }
}