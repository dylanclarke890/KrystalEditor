#include <Graphics/Renderer/Renderer2D.h>
#include <Graphics/Camera/Orthographic.h>
#include <Graphics/Camera/Perspective.h>
#include <Graphics/Colors.h>

#include <Misc/Performance.h>
#include <Misc/Time.h>
#include <Misc/Chrono.h>
#include <Misc/Random.h>

#include "KrystalEditor.h"

namespace Krys
{
  static uint ModelCount = 1000;
  static Mat4 *ModelMatrices = new Mat4[ModelCount];

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
    Random::Init();

    Window->SetEventCallback(KRYS_BIND_EVENT_FN(KrystalEditor::OnEvent));

    Context->SetDepthTestingEnabled(true);
    Context->SetDepthTestFunc(DepthTestFunc::Less);

    auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f, 0.1f, 1000.0f);
    camera->SetYaw(160.0f);
    camera->SetPitch(-30.0f);
    camera->SetPosition(Vec3(0.0f, 5.0f, -10.0f));
    Camera = camera;
    auto cameraController = CreateRef<PerspectiveCameraController>(camera);
    cameraController->SetSpeed(10.0f);
    CameraController = cameraController;

    Textures["crate"] = Context->CreateTexture2D("textures/crate.png");

    float skyboxVertices[108] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};
    VertexArrays["skybox"] = Context->CreateVertexArray();
    VertexBuffers["skybox"] = Context->CreateVertexBuffer(skyboxVertices, sizeof(skyboxVertices));
    VertexBuffers["skybox"]->SetLayout(VertexBufferLayout({{ShaderDataType::Float3, "a_Position"}}));
    VertexArrays["skybox"]->AddVertexBuffer(VertexBuffers["skybox"]);

    Shaders["skybox"] = Context->CreateShader("shaders/skybox/v.vert", "shaders/skybox/f.frag");
    Cubemaps["skybox"] = Context->CreateTextureCubemap({"cubemaps/space-skybox/right.png",
                                                        "cubemaps/space-skybox/left.png",
                                                        "cubemaps/space-skybox/top.png",
                                                        "cubemaps/space-skybox/bottom.png",
                                                        "cubemaps/space-skybox/front.png",
                                                        "cubemaps/space-skybox/back.png"});

    auto multisampleFramebuffer = Context->CreateFramebuffer(Window->GetWidth(), Window->GetHeight(), 4);
    Framebuffers["multisample"] = multisampleFramebuffer;
    multisampleFramebuffer->AddColorAttachment();
    multisampleFramebuffer->AddDepthStencilAttachment();

    Framebuffers["resolved"] = Context->CreateFramebuffer(Window->GetWidth(), Window->GetHeight(), 1);
    Framebuffers["resolved"]->AddColorAttachment();

    Shaders["post-processing"] = Context->CreateShader("shaders/renderer-2d/v.vert", "shaders/basic/scene-texture.frag");
  }

  void KrystalEditor::Update(float dt)
  {
    static auto objectMaterial = CreateRef<Material>(Textures["crate"]);
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f), Vec3(45.0f));

    static auto stageMaterial = CreateRef<Material>(Framebuffers["resolved"]->GetColorAttachment());
    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));

    Window->BeginFrame();
    Input::BeginFrame();
    {
      KRYS_PERFORMANCE_TIMER("Frame")
      CameraController->OnUpdate(Time::GetDeltaSecs());

      Framebuffers["resolved"]->Bind();
      Context->Clear(RenderBuffer::Color);
      Framebuffers["multisample"]->Bind(FramebufferBindType::ReadAndDraw);
      Context->Clear(RenderBuffer::Color | RenderBuffer::Depth);

      Renderer2D::BeginScene(Camera);
      {
        Renderer2D::DrawCube(objectTransform, Colors::Green);
        Renderer2D::DrawCube(stageTransform, Colors::Gray50);
      }
      Renderer2D::EndScene();
      RectBounds bounds = {static_cast<float>(0), static_cast<float>(Window->GetWidth()), static_cast<float>(0), static_cast<float>(Window->GetHeight())};
      Framebuffers["multisample"]->BlitTo(Framebuffers["resolved"], bounds, bounds, RenderBuffer::Color);
      Context->Clear(RenderBuffer::Color | RenderBuffer::Depth);

      Renderer2D::BeginScene(Camera, Shaders["post-processing"]);
      {
        Renderer2D::DrawCube(objectTransform, Colors::Green);
        Renderer2D::DrawCube(stageTransform, stageMaterial);
      }
      Renderer2D::EndScene();

      Framebuffers["multisample"]->BlitToScreen(bounds, bounds, RenderBuffer::Color);

#if 0
      // Context->SetDepthTestFunc(DepthTestFunc::EqualOrLess);
      // {
      //   Shaders["skybox"]->Bind();
      //   auto view = Mat4(Mat3(Camera->GetView()));
      //   auto viewProjection = Camera->GetProjection() * view;
      //   Shaders["skybox"]->SetUniform("u_ViewProjection", viewProjection);
      //   VertexArrays["skybox"]->Bind();
      //   Cubemaps["skybox"]->Bind();
      //   Context->DrawVertices(36);
      // }
      // Context->SetDepthTestFunc(DepthTestFunc::Less);
#endif
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