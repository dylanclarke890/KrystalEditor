#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <Core/Logger.hpp>
#include <Graphics/Colors.hpp>
#include <IO/IO.hpp>

#include "Pong.hpp"

namespace Krys
{
  Gfx::VertexBufferHandle triangleBufferHandle;
  Gfx::PipelineHandle trianglePipelineHandle;

  static Gfx::ShaderHandle LoadShader(Ptr<Gfx::GraphicsContext> context, Gfx::ShaderStage stage,
                                      const string &filepath) noexcept
  {
    auto source = IO::ReadFileText(filepath);
    auto shaderDescription = Gfx::ShaderDescription {stage, source};
    auto shader = context->CreateShader(shaderDescription);
    return shader;
  }

  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context) noexcept
      : Application(std::move(context)), _game(CreateUnique<Pong>(_context.get()))
  {
    _context->GetEventManager()->RegisterHandler<KeyboardEvent>(
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

    auto graphicsContext = _context->GetGraphicsContext();
    graphicsContext->SetClearColor(Colors::Olive);

    float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
    triangleBufferHandle = graphicsContext->CreateVertexBuffer(sizeof(vertices));

    auto &buffer = graphicsContext->GetVertexBuffer(triangleBufferHandle);
    buffer.SetData(vertices, sizeof(vertices));

    auto vertexShader = LoadShader(graphicsContext, Gfx::ShaderStage::Vertex, "shaders/triangle.vert");
    auto fragmentShader = LoadShader(graphicsContext, Gfx::ShaderStage::Fragment, "shaders/triangle.frag");

    trianglePipelineHandle = graphicsContext->CreatePipeline();
    auto &pipeline = graphicsContext->GetPipeline(trianglePipelineHandle);

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

    auto ctx = _context->GetGraphicsContext();
    ctx->Clear();
    ctx->DrawArrays(trianglePipelineHandle, triangleBufferHandle, PrimitiveType::Triangles, 0, 3);
  }
}