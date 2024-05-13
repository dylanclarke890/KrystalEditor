#include <Graphics/Renderer2D.h>
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

    Camera = CreateRef<PerspectiveCamera>(
        45.0f,
        static_cast<float>(Window->GetWidth()) / static_cast<float>(Window->GetHeight()),
        0.1f, 100.0f);
    Camera->SetPosition({0.0f, 0.0f, 3.0f});
  }

  void KrystalEditor::Update(float dt)
  {
    static auto size = Vec2(0.5f);
    static auto pos1 = Vec3(-0.75f, +0.0f, 0.0f);
    static auto pos2 = Vec3(+0.25f, +0.0f, 0.0f);
    static auto pos3 = Vec3(+0.00f, -0.20f, 0.0f);
    static auto pos4 = Vec3(+0.00f, -0.80f, 0.0f);
    static auto color1 = Vec4(0.8f, 0.6f, 0.7f, 1.0f);
    static auto color2 = Vec4(0.3f, 0.2f, 0.4f, 1.0f);
    static auto color3 = Vec4(color1.x, color1.y, color1.z, 0.5f);
    static auto texture = Context->CreateTexture2D("textures/container.jpg");
    texture->GenerateMipmaps();
    static auto subTextureSpriteSize = Vec2(128.0f, 128.0f);
    static auto subTextureCellSize = Vec2(1.0f, 1.0f);
    static auto subTextureCoords = Vec2(0.25f, 1.0f);
    static auto subTexture = Context->CreateSubTexture2D(texture, subTextureCoords,
                                                         subTextureSpriteSize, subTextureCellSize);
    Window->BeginFrame();
    Input::BeginFrame();
    {
      Camera->OnUpdate(Time::GetDeltaSecs());
      Renderer2D::BeginScene(Camera);
      {
        Context->Clear(ClearFlags::Color);
        Renderer2D::DrawQuad(pos1, size, color1);
        Renderer2D::DrawQuad(pos2, size, texture);
        Renderer2D::DrawQuad(pos3, size, texture, color3);
        Renderer2D::DrawQuad(pos4, size, subTexture);
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