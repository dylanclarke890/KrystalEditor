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
  static auto lightSourcePosition = Vec3(1.0f, 3.0f, -3.0f);

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

    camera->SetPosition(Vec3(-2.0f, 1.8f, 6.0f));
    camera->SetPitch(-5.0f);
    camera->SetYaw(25.0f);

    Vec3 lightAmbient = {0.2f, 0.2f, 0.2f}; // darken diffuse light a bit
    Vec3 lightDiffuse = {0.5f, 0.5f, 0.5f};
    Vec3 lightSpecular = {1.0f, 1.0f, 1.0f};
    Renderer2D::SetLightSourcePosition(lightSourcePosition);
    Renderer2D::SetLightSourceAmbient(lightAmbient);
    Renderer2D::SetLightSourceDiffuse(lightDiffuse);
    Renderer2D::SetLightSourceSpecular(lightSpecular);

    Vec3 materialDiffuse = {1.0f, 0.5f, 0.81f};
    Vec3 materialSpecular = {0.5f, 0.5f, 0.5f};
    Renderer2D::SetMaterialDiffuse(materialDiffuse);
    Renderer2D::SetMaterialAmbient(materialDiffuse);
    Renderer2D::SetMaterialSpecular(materialSpecular);
    Renderer2D::SetMaterialShine(256.0f);
  }

  void KrystalEditor::Update(float dt)
  {
    static auto lightSourceSize = Vec3(1.0f, 1.0f, 1.0f);

    static auto stagePosition = Vec3(3.0f, -0.5f, 0.0f);
    static auto stageSize = Vec3(15.0f, 0.25f, 15.0f);

    static auto objectPosition = Vec3(1.0f, 0.0f, 0.0f);
    static auto objectSize = Vec3(1.0f, 1.0f, 1.0f);

    Window->BeginFrame();
    Input::BeginFrame();
    {
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Renderer2D::BeginScene(Camera);
      {
        Context->Clear(ClearFlags::Color | ClearFlags::Depth);

        Renderer2D::DrawCube(stagePosition, stageSize, Colors::White);
        Renderer2D::DrawCube(objectPosition, objectSize, Colors::Coral);

        Renderer2D::DrawLightSourceCube(lightSourcePosition, lightSourceSize);
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