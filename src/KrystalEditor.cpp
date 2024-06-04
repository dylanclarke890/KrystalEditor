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
    camera->SetYaw(105.0f);
    camera->SetPitch(-60.0f);
    camera->SetPosition(Vec3(-70.0f, 100.0f, 0.0f));
    Camera = camera;
    CameraController = CreateRef<PerspectiveCameraController>(camera);

    Shaders["default"] = Context->CreateShader("shaders/instanced-drawing/v.vert", "shaders/instanced-drawing/f.frag");
    Shaders["model"] = Context->CreateShader("shaders/models/v.vert", "shaders/models/f.frag");

    float quadVertices[] = {
        // positions     // colors
        -0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
        0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,

        -0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
        0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.0f, 1.0f, 1.0f};

    VertexArrays["default"] = Context->CreateVertexArray();

    VertexBuffers["default"] = Context->CreateVertexBuffer(quadVertices, sizeof(quadVertices));
    VertexBuffers["default"]->SetLayout(
        VertexBufferLayout(
            {{ShaderDataType::Float2, "aPos"},
             {ShaderDataType::Float3, "aColor"}}));
    VertexArrays["default"]->AddVertexBuffer(VertexBuffers["default"]);

    Textures["crate"] = Context->CreateTexture2D("textures/crate.png");

    Models["asteroid"] = CreateRef<Model>("models/asteroid/rock.obj");
    Models["planet"] = CreateRef<Model>("models/mars/planet.obj");

    srand(Random::UInt()); // initialize random seed

    float radius = 50.0;
    float offset = 2.5f;
    for (uint i = 0; i < ModelCount; i++)
    {
      Mat4 model = Mat4(1.0f);
      // 1. translation: displace along circle with 'radius' in range [-offset, offset]
      float angle = (float)i / (float)ModelCount * 360.0f;
      float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
      float x = sin(angle) * radius + displacement;
      displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
      float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
      displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
      float z = cos(angle) * radius + displacement;
      model = glm::translate(model, Vec3(x, y, z));

      // 2. scale: scale between 0.05 and 0.25f
      float scale = (rand() % 20) / 100.0f + 0.05f;
      model = glm::scale(model, Vec3(scale));

      // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
      float rotationAngle = Random::Float(0.0f, 360.0f);
      model = glm::rotate(model, rotationAngle, Vec3(0.4f, 0.6f, 0.8f));

      // 4. now add to list of matrices
      ModelMatrices[i] = model;
    }

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
  }

  void KrystalEditor::Update(float dt)
  {
    static auto objectMaterial = CreateRef<Material>(Textures["crate"]);
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f));

    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto planetTransform = CreateRef<Transform>(Vec3(0.0f, -3.0f, 0.0f), Vec3(4.0f));
    Window->BeginFrame();
    Input::BeginFrame();
    {
      KRYS_PERFORMANCE_TIMER("Frame")
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Context->Clear(ClearFlags::Color | ClearFlags::Depth);

      auto modelShader = Shaders["model"];
      Renderer2D::BeginScene(Camera, modelShader);
      {

        modelShader->Bind();
        modelShader->SetUniform("u_Model", planetTransform->GetModel());
        Models["planet"]->Draw(modelShader);

        for (uint i = 0; i < ModelCount; i++)
        {
          Shaders["model"]->SetUniform("u_Model", ModelMatrices[i]);
          Models["asteroid"]->Draw(Shaders["model"]);
        }
      }
      Renderer2D::EndScene();

      Context->SetDepthTestFunc(DepthTestFunc::EqualOrLess);
      {
        Shaders["skybox"]->Bind();
        auto view = Mat4(Mat3(Camera->GetView()));
        auto viewProjection = Camera->GetProjection() * view;
        Shaders["skybox"]->SetUniform("u_ViewProjection", viewProjection);
        VertexArrays["skybox"]->Bind();
        Cubemaps["skybox"]->Bind();
        Context->DrawVertices(36);
      }
      Context->SetDepthTestFunc(DepthTestFunc::Less);
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