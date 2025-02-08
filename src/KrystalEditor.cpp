#include "KrystalEditor.hpp"
#include <Core/Platform.hpp>
#include <Debug/Macros.hpp>
#include <Events/Input/KeyboardEvent.hpp>
#include <Events/Input/MouseMoveEvent.hpp>
#include <Events/Input/ScrollWheelEvent.hpp>
#include <Events/QuitEvent.hpp>
#include <Graphics/Colours.hpp>
#include <Graphics/OpenGL/OpenGLTexture.hpp>
#include <Graphics/Scene/MaterialNode.hpp>
#include <Graphics/Scene/MeshNode.hpp>
#include <Graphics/Transform.hpp>
#include <IO/Images.hpp>
#include <IO/IO.hpp>
#include <IO/Logger.hpp>
#include <MTL/Common/Convert.hpp>
#include <MTL/Matrices/Ext/Transformations.hpp>
#include <MTL/Vectors/Ext/Format.hpp>

#include "Pong.hpp"

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context) noexcept
      : Application(std::move(context)), _game(CreateUnique<Pong>(_context.get())),
        _camera(Gfx::CameraType::Perspective, 1'920, 1'080, 100, Vec3(0.0f), 10.0f)
  {
  }

  void KrystalEditor::OnInit() noexcept
  {
    _context->GetWindowManager()->GetCurrentWindow()->ShowCursor(true);
    // TODO: this doesn't seem to work
    // _context->GetWindowManager()->GetCurrentWindow()->LockCursor(true);

    BindEvents();

    auto graphicsContext = _context->GetGraphicsContext();
    graphicsContext->SetClearColour(Gfx::Colours::Black);

    auto mesh = _context->GetMeshManager()->CreateCube(Gfx::Colours::White);
    auto vertexShader =
      graphicsContext->CreateShader(Gfx::ShaderStage::Vertex, IO::ReadFileText("shaders/phong.vert"));
    auto fragmentShader =
      graphicsContext->CreateShader(Gfx::ShaderStage::Fragment, IO::ReadFileText("shaders/phong.frag"));

    auto shader = graphicsContext->CreateProgram(vertexShader, fragmentShader);
    auto texture = _context->GetTextureManager()->LoadTexture("textures/wood-wall.jpg");
    auto material = _context->GetMaterialManager()->CreateMaterial<Gfx::PhongMaterial>(shader, texture);

    // auto &material = *_context->GetMaterialManager()->GetMaterial<Gfx::PhongMaterial>(_material);
    // material.SetAmbientColour(Gfx::Colours::Lime);

    auto *sm = _context->GetSceneGraphManager();
    sm->CreateScene("main");

    auto *root = sm->GetScene("main")->GetRoot();

    {
      auto meshNode = CreateRef<Gfx::MeshNode>(mesh, Gfx::Transform());
      meshNode->AddChild(CreateRef<Gfx::MaterialNode>(material));
      root->AddChild(meshNode);
    }

    {
      auto transform = Gfx::Transform {};
      transform.SetTranslation({1.0f, 0.5f, 0.5f});

      auto meshNode = CreateRef<Gfx::MeshNode>(mesh, transform);
      meshNode->AddChild(CreateRef<Gfx::MaterialNode>(material));
      root->AddChild(meshNode);
    }
  }

  void KrystalEditor::OnShutdown() noexcept
  {
  }

  void KrystalEditor::OnRender() noexcept
  {
    using namespace Gfx;

    auto ctx = _context->GetGraphicsContext();
    auto renderer = _context->GetRenderer();

    ctx->Clear(ClearBuffer::Colour | ClearBuffer::Depth);
    renderer->Render(_context->GetSceneGraphManager()->GetScene("main"), _camera);
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
  }

  void KrystalEditor::OnFixedUpdate(float) noexcept
  {
  }

  void KrystalEditor::BindEvents() noexcept
  {
    auto *em = _context->GetEventManager();
    em->RegisterHandler<KeyboardEvent>(
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

    em->RegisterHandler<QuitEvent>(
      [&](const QuitEvent &)
      {
        _running = false;
        return true;
      });

    em->RegisterHandler<ScrollWheelEvent>(
      [&](const ScrollWheelEvent &event)
      {
        _camera.Zoom(event.Delta());
        return false;
      });
  }
}