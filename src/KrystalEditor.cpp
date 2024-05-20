#include <Graphics/Renderer2D.h>
#include <Graphics/Camera/Orthographic.h>
#include <Graphics/Camera/Perspective.h>
#include <Graphics/Colors.h>

#include <Misc/Performance.h>
#include <Misc/Time.h>
#include <Misc/Chrono.h>

#include "KrystalEditor.h"

namespace Krys
{
  KrystalEditor::KrystalEditor()
      : Application("Krystal Editor", 1280, 720, 60.0f),
        WireFrameMode(false) {}

  void KrystalEditor::Startup()
  {
    Application::Startup();

    Renderer2D::Init(Context);

    Window->SetEventCallback(KRYS_BIND_EVENT_FN(KrystalEditor::OnEvent));

    Context->SetDepthTestingEnabled(true);
    Context->SetDepthTestFunc(DepthTestFunc::Less);

    auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f);
    Camera = camera;
    CameraController = CreateRef<PerspectiveCameraController>(camera);

    camera->SetYaw(25.0f);
    camera->SetPitch(-40.0f);
    camera->SetPosition(Vec3(-1.0f, 7.0f, 10.0f));

    Renderer2D::SetLightSourceAmbient({0.2f, 0.2f, 0.2f});
    Renderer2D::SetLightSourceDiffuse({0.5f, 0.5f, 0.5f});
    Renderer2D::SetLightSourceSpecular({1.0f, 1.0f, 1.0f});
    Renderer2D::SetLightSourceAttenuation(1.0f, 0.09f, 0.032f);
  }

  void KrystalEditor::Update(float dt)
  {
    // static auto stageTransform = CreateRef<Transform>(Vec3(3.0f, -0.5f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
    static auto objectTexture = Context->CreateTexture2D("textures/crate.png");
    static auto objectSpecularTexture = Context->CreateTexture2D("textures/crate-specular.png");
    static auto objectEmissionTexture = Context->CreateTexture2D("textures/crate-emission.png");
    static auto objectMaterial = CreateRef<Material>(objectTexture, objectSpecularTexture, objectEmissionTexture);
    objectMaterial->Shininess = 128.0f;

    static auto lightSourceTransform = CreateRef<Transform>(Vec3(-0.2f, -1.0f, -0.3f), Vec3(1.0f, 1.0f, 1.0f));
    static auto lightMoveSpeed = 0.01f;

    Window->BeginFrame();
    Input::BeginFrame();
    {
      KRYS_PERFORMANCE_TIMER("Update");

      if (Input::IsKeyPressed(KeyCode::LeftArrow))
        lightSourceTransform->Position.x -= lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::RightArrow))
        lightSourceTransform->Position.x += lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::UpArrow))
        lightSourceTransform->Position.y += lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::DownArrow))
        lightSourceTransform->Position.y -= lightMoveSpeed * dt;

      Renderer2D::SetLightSourcePosition(Vec4(lightSourceTransform->Position, 0.0f));

      CameraController->OnUpdate(Time::GetDeltaSecs());
      Renderer2D::BeginScene(Camera);
      {
        Context->Clear(ClearFlags::Color | ClearFlags::Depth);

        Vec3 cubePositions[] = {
            Vec3(0.0f, 0.0f, 0.0f),
            Vec3(2.0f, 5.0f, -15.0f),
            Vec3(-1.5f, -2.2f, -2.5f),
            Vec3(-3.8f, -2.0f, -12.3f),
            Vec3(2.4f, -0.4f, -3.5f),
            Vec3(-1.7f, 3.0f, -7.5f),
            Vec3(1.3f, -2.0f, -2.5f),
            Vec3(1.5f, 2.0f, -2.5f),
            Vec3(1.5f, 0.2f, -1.5f),
            Vec3(-1.3f, 1.0f, -1.5f)};

        // Renderer2D::DrawCube(stageTransform, Colors::Gray50);
        for (int i = 0; i < 10; i++)
        {
          objectTransform->Position = cubePositions[i];
          float angle = 20.0f * i;
          objectTransform->Rotation = Vec3(1.0f, 0.3f, 0.5f) * angle;
          Renderer2D::DrawCube(objectTransform, objectMaterial);
        }

        Renderer2D::DrawLightSourceCube(lightSourceTransform);
      }
      Renderer2D::EndScene();
    }
    Input::EndFrame();
    Window->EndFrame();
  }

  void KrystalEditor::OnEvent(Event &event)
  {
    CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(KRYS_BIND_EVENT_FN(KrystalEditor::OnKeyPressEvent));

    Application::OnEvent(event);
  }

  bool KrystalEditor::OnKeyPressEvent(KeyPressedEvent &event)
  {
    switch (event.Key)
    {
    case KeyCode::Space:
    {
      WireFrameMode = !WireFrameMode;
      Context->SetWireframeModeEnabled(WireFrameMode);
      break;
    }
    case KeyCode::Escape:
    {
      StopRunning();
      break;
    }
    default:
      break;
    }

    return false;
  }

  bool KrystalEditor::OnShutdownEvent(ShutdownEvent &event)
  {
    Shutdown();
    return false;
  }

  void KrystalEditor::Shutdown()
  {
    Renderer2D::Shutdown();
    Application::Shutdown();
  }
}