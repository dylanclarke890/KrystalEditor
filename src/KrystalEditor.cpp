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
      _triangleMesh = _context->GetMeshManager()->CreateMesh(
        {VertexData {Vec3 {0.5f, 0.5f, 0.0f}, Colours::Cyan, Vec2 {1.0f, 1.0f}},
         VertexData {Vec3 {0.5f, -0.5f, 0.0f}, Colours::Beige, Vec2 {1.0f, 0.0f}},
         VertexData {Vec3 {-0.5f, -0.5f, 0.0f}, Colours::Red, Vec2 {0.0f, 0.0f}},
         VertexData {Vec3 {-0.5f, 0.5f, 0.0f}, Colours::Green, Vec2 {0.0f, 1.0f}}},
        {0, 1, 3, 1, 2, 3});
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

    _texture = _context->GetTextureManager()->LoadTexture("textures/wood-wall.jpg");

    _textureUniform = Gfx::OpenGL::OpenGLUniform<uint64>(_triangleShader, "u_Texture");
    _transformUniform = Gfx::OpenGL::OpenGLUniform<Mat4>(_triangleShader, "u_Transform");

    auto &glTexture =
      static_cast<Gfx::OpenGL::OpenGLTexture &>(_context->GetTextureManager()->GetTexture(_texture));
    _textureUniform.SetValue(glTexture.GetNativeBindlessHandle());
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
    Mat4 trans = Mat4(1.0f);
    trans = MTL::Rotate(trans, static_cast<float>(Platform::GetTime()), Vec3(0.0, 0.0, 1.0));
    trans = MTL::Scale(trans, Vec3(0.5, 0.5, 0.5));

    _transformUniform.SetValue(trans);
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