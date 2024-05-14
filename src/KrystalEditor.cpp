#include <Graphics/Renderer2D.h>
#include <Graphics/Camera/Orthographic.h>
#include <Graphics/Camera/Perspective.h>

#include <Misc/Performance.h>
#include <Misc/Time.h>
#include <Misc/Chrono.h>

#include "KrystalEditor.h"

#define TEST_ORTHO_CAMERA 1

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

#if (TEST_ORTHO_CAMERA)
    Camera = CreateRef<OrthographicCamera>(Window->GetWidth(), Window->GetHeight());
#else
    Camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f);
    Camera->SetPosition({0.0f, 0.0f, 1.0f});
#endif
  }

  void KrystalEditor::Update(float dt)
  {
#if (TEST_ORTHO_CAMERA)
    static auto size = Vec2(100.0f);
    static auto pos = Vec3(100.0f, 100.0f, 0.0f);
    static auto color = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
#else
    static auto size = Vec2(1.0f);
    static auto pos = Vec3(0.0f, 0.0f, 0.0f);
    static auto color = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
#endif

    Window->BeginFrame();
    Input::BeginFrame();
    {
      Camera->OnUpdate(Time::GetDeltaSecs());
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
    Camera->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(KRYS_BIND_EVENT_FN(KrystalEditor::OnKeyPressEvent));

    Application::OnEvent(event);
  }

  bool KrystalEditor::OnKeyPressEvent(KeyPressedEvent &event)
  {
    if (event.Key == KeyCode::Space)
    {
      WireFrameMode = !WireFrameMode;
      Context->SetWireframeModeEnabled(WireFrameMode);
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