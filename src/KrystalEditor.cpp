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
      Camera->SetPosition({0.0f, 0.0f, 1.0f});
      CameraController = CreateRef<PerspectiveCameraController>(camera);
    }
  }

  void KrystalEditor::Update(float dt)
  {
    Vec2 size;
    Vec3 pos;
    Vec4 color;
    if (UseOrthographicCamera)
    {
      size = Vec2(100.0f);
      pos = Vec3(100.0f, 100.0f, 0.0f);
      color = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    else
    {
      size = Vec2(1.0f);
      pos = Vec3(0.0f, 0.0f, 0.0f);
      color = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }

    Window->BeginFrame();
    Input::BeginFrame();
    {
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Renderer2D::BeginScene(Camera);
      {
        Context->Clear(ClearFlags::Color);
        Renderer2D::DrawQuad(pos, size, color, 45.0f);
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