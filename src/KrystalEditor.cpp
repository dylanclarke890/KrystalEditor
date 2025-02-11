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
  static Gfx::LightHandle directionalLightHandle;

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

    auto *sm = _context->GetSceneGraphManager();
    sm->CreateScene("main");

    auto *root = sm->GetScene("main")->GetRoot();

    {
      auto *mm = _context->GetMaterialManager();
      auto *tm = _context->GetTextureManager();

      auto texture = tm->LoadTexture("textures/wood-crate.png");
      auto materialHandle = mm->CreatePhongMaterial(texture);
      auto material = mm->GetMaterial<Gfx::PhongMaterial>(materialHandle);
      material->SetSpecularTexture(tm->LoadTexture("textures/wood-crate-specular.png"));
      // material->SetEmissionTexture(tm->LoadTexture("textures/matrix.jpg"));

      auto transform = Gfx::Transform {};
      transform.SetTranslation({0.0f, 0.0f, 0.0f});

      auto meshNode = CreateRef<Gfx::MeshNode>(mesh, transform);
      meshNode->AddChild(CreateRef<Gfx::MaterialNode>(materialHandle));
      root->AddChild(meshNode);

      transform.SetTranslation({0.0f, 0.0f, -2.0f});
      auto meshNodes2 = CreateRef<Gfx::MeshNode>(mesh, transform);
      materialHandle = mm->CreatePhongMaterial(Gfx::PhongMaterials::WhitePlastic);
      // material = mm->GetMaterial<Gfx::PhongMaterial>(materialHandle);
      // material->SetEmissionTexture(tm->LoadTexture("textures/matrix.jpg"));
      meshNodes2->AddChild(CreateRef<Gfx::MaterialNode>(materialHandle));
      root->AddChild(meshNodes2);
    }

    {
      auto *lm = _context->GetLightManager();
      directionalLightHandle =
        lm->CreateLight<Gfx::DirectionalLight>(Gfx::Colour {1.f, 1.f, 1.f}, Vec3 {0.0f, 1.0f, 0.0f});
      root->AddChild(CreateRef<Gfx::LightNode>(directionalLightHandle));

      // root->AddChild(CreateRef<Gfx::LightNode>(
      //   lm->CreateLight<Gfx::PointLight>(Gfx::Colour {0.0f, 0.0f, 1.0f}, Vec3 {1.2f, 2.5f, -2.0f})));

      // root->AddChild(CreateRef<Gfx::LightNode>(
      //   lm->CreateLight<Gfx::PointLight>(Gfx::Colour {0.0f, 1.0f, 0.0f}, Vec3 {1.2f, -2.5f, 2.0f})));
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

    auto &light = *_context->GetLightManager()->GetLight<Gfx::DirectionalLight>(directionalLightHandle);
    light.SetDirection(_camera.GetForward());

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