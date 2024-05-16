#include <Graphics/Renderer2D.h>
#include <Graphics/Camera/Orthographic.h>
#include <Graphics/Camera/Perspective.h>

#include <Misc/Performance.h>
#include <Misc/Time.h>
#include <Misc/Chrono.h>

#include "KrystalEditor.h"

namespace Krys
{
  KrystalEditor::KrystalEditor()
      : Application("Krystal Editor", 1280, 720, 60.0f),
        WireFrameMode(false), UseOrthographicCamera(false) {}

  void KrystalEditor::Startup()
  {
    Application::Startup();

    Renderer2D::Init(Context);
    Window->SetEventCallback(KRYS_BIND_EVENT_FN(KrystalEditor::OnEvent));
    SetCamera();
  }

  void KrystalEditor::SetCamera()
  {
    if (UseOrthographicCamera)
    {
      auto camera = CreateRef<OrthographicCamera>(Window->GetWidth(), Window->GetHeight());
      Camera = camera;
      CameraController = CreateRef<OrthographicCameraController>(camera);
    }
    else
    {
      auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f);
      Camera = camera;
      CameraController = CreateRef<PerspectiveCameraController>(camera);
    }
  }

  void KrystalEditor::Update(float dt)
  {
    static auto texture = Context->CreateTexture2D("textures/container.jpg");
    Vec3 quadPos1;
    Vec3 quadPos2;
    Vec2 quadSize;

    Vec3 cubePos1;
    Vec3 cubePos2;
    Vec3 cubeSize;

    Vec4 cubeColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    Vec4 quadColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

    if (UseOrthographicCamera)
    {
      quadPos1 = Vec3(100.0f, 100.0f, 0.0f);
      quadPos2 = Vec3(100.0f, 100.0f, 0.0f);
      quadSize = Vec2(100.0f);

      cubePos1 = Vec3(100.0f, 100.0f, -1.0f);
      cubePos2 = Vec3(300.0f, 100.0f, 0.0f);
      cubeSize = Vec3(100.0f);
    }
    else
    {
      quadPos1 = Vec3(-1.5f, 0.0f, 0.0f);
      cubePos1 = Vec3(0.0f, 0.0f, 0.0f);
      quadSize = Vec2(1.0f);

      quadPos2 = Vec3(4.5f, 0.0f, 0.0f);
      cubePos2 = Vec3(3.0f, 0.0f, 0.0f);
      cubeSize = Vec3(1.0f);
    }

    Window->BeginFrame();
    Input::BeginFrame();
    {
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Renderer2D::BeginScene(Camera);
      {
        Context->Clear(ClearFlags::Color | ClearFlags::Depth);
        Renderer2D::DrawQuad(quadPos1, quadSize, quadColor);
        Renderer2D::DrawQuad(quadPos2, quadSize, texture);
        Renderer2D::DrawCube(cubePos1, cubeSize, cubeColor);
        Renderer2D::DrawCube(cubePos2, cubeSize, texture);
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
    switch (event.Key) {
     case KeyCode::Space:
      {
       WireFrameMode = !WireFrameMode;
       Context->SetWireframeModeEnabled(WireFrameMode);
       break;
     }
     case KeyCode::O:
     {
       UseOrthographicCamera = true;
       SetCamera();
       break;
     }
     case KeyCode::P:
     {
       UseOrthographicCamera = false;
       SetCamera();
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