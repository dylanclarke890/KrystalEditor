#include "KrystalEditor.hpp"
#include <Core/Platform.hpp>
#include <Debug/Macros.hpp>
#include <Events/Input/KeyboardEvent.hpp>
#include <Events/Input/MouseMoveEvent.hpp>
#include <Events/Input/ScrollWheelEvent.hpp>
#include <Events/QuitEvent.hpp>
#include <Graphics/Colours.hpp>
#include <Graphics/Lights/DirectionalLight.hpp>
#include <Graphics/Lights/PointLight.hpp>
#include <Graphics/Lights/SpotLight.hpp>
#include <Graphics/Materials/PhongMaterials.hpp>
#include <Graphics/OpenGL/OpenGLTexture.hpp>
#include <Graphics/Scene/LightNode.hpp>
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
  static Gfx::LightHandle spotLightHandle;

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
    graphicsContext->SetClearColour(Gfx::Colours::Gray25);

    // auto mesh = _context->GetMeshManager()->CreateCube();

    auto *sm = _context->GetSceneGraphManager();
    sm->CreateScene("main");

    auto *root = sm->GetScene("main")->GetRoot();

    auto teapotLoaderFlags = Gfx::ModelLoaderFlags::GenerateFaceNormals | Gfx::ModelLoaderFlags::Triangulate
                             | Gfx::ModelLoaderFlags::DeduplicateVertices;
    auto model =
      _context->GetModelManager()->LoadModel("models/dragon/dragon.obj", teapotLoaderFlags).value();
    {
      auto transform = Gfx::Transform {};
      transform.SetTranslation({0.0f, 0.0f, 0.0f});

      auto meshNode = CreateRef<Gfx::MeshNode>(model.Renderables[0].Mesh, transform);
      meshNode->AddChild(CreateRef<Gfx::MaterialNode>(model.Renderables[0].Material));
      root->AddChild(meshNode);
    }

    {
      auto *lm = _context->GetLightManager();
      spotLightHandle = lm->CreateLight<Gfx::SpotLight>(Gfx::Colour {1.f, 1.f, 1.f}, Vec3 {0.f, 1.f, 0.f},
                                                        Vec3 {0.f, -1.f, 0.f});
      root->AddChild(CreateRef<Gfx::LightNode>(spotLightHandle));

      root->AddChild(CreateRef<Gfx::LightNode>(
        lm->CreateLight<Gfx::PointLight>(Gfx::Colour {1.0f, 1.0f, 1.0f}, Vec3 {1.2f, 2.5f, -2.0f})));

      root->AddChild(CreateRef<Gfx::LightNode>(
        lm->CreateLight<Gfx::PointLight>(Gfx::Colour {1.0f, 1.0f, 1.0f}, Vec3 {1.2f, -2.5f, 2.0f})));
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

    auto &light = *_context->GetLightManager()->GetLight<Gfx::SpotLight>(spotLightHandle);
    light.SetDirection(_camera.GetForward());
    light.SetPosition(_camera.GetPosition());

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