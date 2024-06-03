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

    auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f);
    camera->SetYaw(25.0f);
    camera->SetPitch(-40.0f);
    camera->SetPosition(Vec3(-1.0f, 7.0f, 10.0f));
    Camera = camera;
    CameraController = CreateRef<PerspectiveCameraController>(camera);

    TestShader = Context->CreateShader("shaders/geo-shader-test/vertex.vert", "shaders/geo-shader-test/fragment.frag", "shaders/geo-shader-test/geometry.geo");

    float points[] = {
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // top-left
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // top-right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f // bottom-left
    };
    TestVertexArray = Context->CreateVertexArray();
    TestVertexBuffer = Context->CreateVertexBuffer(points, sizeof(points));
    TestVertexBuffer->SetLayout(BufferLayout(sizeof(points), {{ShaderDataType::Float2, "aPos"}, {ShaderDataType::Float3, "aColor"}}));
    TestVertexArray->AddVertexBuffer(TestVertexBuffer);
  }

  void KrystalEditor::Update(float dt)
  {
    static auto objectTexture = Context->CreateTexture2D("textures/crate.png");
    static auto objectMaterial = CreateRef<Material>(objectTexture);
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f));

    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto skyboxCrateTransform = CreateRef<Transform>(Vec3(0.0f), Vec3(1.0f));

    Window->BeginFrame();
    Input::BeginFrame();
    {
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Context->Clear(ClearFlags::Color | ClearFlags::Depth);

      TestVertexArray->Bind();
      TestShader->Bind();
      Context->DrawVertices(4, DrawMode::Points);
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