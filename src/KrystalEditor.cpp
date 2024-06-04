#include <Graphics/Renderer/Renderer2D.h>
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
        Camera(nullptr), CameraController(nullptr), WireFrameMode(false),
        VertexArrays({}), UniformBuffers({}), VertexBuffers({}), IndexBuffers({}),
        InstanceArrayBuffers({}), Framebuffers({}), Textures({}), Models({}), Shaders({})
  {
  }

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

    Shaders["default"] = Context->CreateShader("shaders/instancing/vertex.vert", "shaders/instancing/fragment.frag");
    Shaders["model"] = Context->CreateShader("shaders/models/test-model.vert", "shaders/models/test-model.frag");

    float quadVertices[] = {
        // positions     // colors
        -0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
        0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,

        -0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
        0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.0f, 1.0f, 1.0f};

    VertexBuffers["default"] = Context->CreateVertexBuffer(quadVertices, sizeof(quadVertices));
    VertexBuffers["default"]->SetLayout(
        VertexBufferLayout(
            {{ShaderDataType::Float2, "aPos"},
             {ShaderDataType::Float3, "aColor"}}));

    Vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
      for (int x = -10; x < 10; x += 2)
      {
        Vec2 translation;
        translation.x = (float)x / 10.0f + offset;
        translation.y = (float)y / 10.0f + offset;
        translations[index++] = translation;
      }
    }
    InstanceArrayBuffers["default"] = Context->CreateInstanceArrayBuffer(translations, sizeof(translations));
    InstanceArrayBuffers["default"]->SetLayout(InstanceArrayBufferLayout({{ShaderDataType::Float2, "aOffset", 1}}));
    InstanceArrayBuffers["default"]->SetData(&translations, sizeof(translations));

    VertexArrays["default"] = Context->CreateVertexArray();
    VertexArrays["default"]->AddVertexBuffer(VertexBuffers["default"]);
    VertexArrays["default"]->AddInstanceArrayBuffer(InstanceArrayBuffers["default"]);

    Textures["crate"] = Context->CreateTexture2D("textures/crate.png");

    Models["asteroid"] = CreateRef<Model>("models/asteroid/rock.obj");
    Models["planet"] = CreateRef<Model>("models/mars/planet.obj");
  }

  void KrystalEditor::Update(float dt)
  {
    static auto objectMaterial = CreateRef<Material>(Textures["crate"]);
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f));

    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto skyboxCrateTransform = CreateRef<Transform>(Vec3(0.0f), Vec3(1.0f));

    Window->BeginFrame();
    Input::BeginFrame();
    {
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Context->Clear(ClearFlags::Color | ClearFlags::Depth);

      VertexArrays["default"]->Bind();
      Shaders["default"]->Bind();
      Context->DrawVerticesInstanced(100, 6, DrawMode::Triangles);
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