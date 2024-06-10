#include <Graphics/Renderer/Renderer.h>
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

    Renderer::Init(Context);
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
    Textures["crate-specular"] = Context->CreateTexture2D("textures/crate-specular.png");

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

    DirectionalLight sampleDirectionalLight;
    sampleDirectionalLight.Ambient = Vec3(0.3f);  // Low ambient light
    sampleDirectionalLight.Diffuse = Vec3(0.5f);  // Moderate diffuse light
    sampleDirectionalLight.Specular = Vec3(1.0f); // Strong specular light
    sampleDirectionalLight.Enabled = false;
    sampleDirectionalLight.Intensity = 1.0f;                    // Full intensity
    sampleDirectionalLight.Direction = Vec3(0.4f, -1.0f, 0.0f); // Direction of the light

    Renderer::Lights.AddDirectionalLight(sampleDirectionalLight);

    PointLight samplePointLight;
    samplePointLight.Ambient = Vec3(0.3f);             // Low ambient light
    samplePointLight.Diffuse = Vec3(0.8f, 0.8f, 0.8f); // Moderate diffuse light
    samplePointLight.Specular = Vec3(1.0f);            // Strong specular light
    samplePointLight.Constant = 1.0f;                  // Constant attenuation term
    samplePointLight.Linear = 0.09f;                   // Linear attenuation term
    samplePointLight.Quadratic = 0.032f;               // Quadratic attenuation term
    samplePointLight.Enabled = false;
    samplePointLight.Intensity = 1.0f;                  // Full intensity
    samplePointLight.Position = Vec3(3.0f, 6.0f, 2.0f); // Position of the point light

    Renderer::Lights.AddPointLight(samplePointLight);

    SpotLight sampleSpotLight;
    sampleSpotLight.Ambient = Vec3(0.2f, 0.2f, 0.2f);  // Low ambient light
    sampleSpotLight.Diffuse = Vec3(0.8f, 0.8f, 0.8f);  // Moderate diffuse light
    sampleSpotLight.Specular = Vec3(1.0f, 1.0f, 1.0f); // Strong specular light
    sampleSpotLight.Constant = 1.0f;                   // Constant attenuation term
    sampleSpotLight.Linear = 0.09f;                    // Linear attenuation term
    sampleSpotLight.Quadratic = 0.032f;                // Quadratic attenuation term
    sampleSpotLight.Enabled = true;
    sampleSpotLight.Intensity = 1.0f;                            // Full intensity
    sampleSpotLight.Direction = Vec3(0.0f, -1.0f, 0.0f);         // Direction of the spotlight
    sampleSpotLight.Position = Vec3(0.0f, 2.0f, 0.0f);           // Position of the spotlight
    sampleSpotLight.InnerCutoff = glm::cos(glm::radians(12.5f)); // Inner cutoff angle (cosine of the angle)
    sampleSpotLight.OuterCutoff = glm::cos(glm::radians(17.5f)); // Outer cutoff angle (cosine of the angle)

    Renderer::Lights.AddSpotLight(sampleSpotLight);

    Shaders["light-source"] = Context->CreateShader("shaders/lighting/light-source.vert", "shaders/lighting/light-source.frag");
  }

  void KrystalEditor::Update(float dt)
  {
    static auto objectMaterial = CreateRef<Material>(Textures["crate"], Textures["crate-specular"]);
    objectMaterial->Shininess = 32.0f;

    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f), Vec3(0.0f));
    static auto lightSourceTransform = CreateRef<Transform>(Vec3(0.0f), Vec3(1.0f));

    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));

    Window->BeginFrame();
    Input::BeginFrame();
    {
      KRYS_PERFORMANCE_TIMER("Frame")
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Context->Clear(RenderBuffer::Color | RenderBuffer::Depth);

      Renderer::BeginScene(Camera);
      {
        Renderer::DrawCube(objectTransform, objectMaterial);
        // Renderer::DrawCube(objectTransform, Colors::Green);
        Renderer::DrawCube(stageTransform, Colors::Gray50);
      }
      Renderer::EndScene();

      Renderer::BeginScene(Camera, Shaders["light-source"]);
      {
        for (auto spotLight : Renderer::Lights.GetPointLights())
        {
          lightSourceTransform->Position = spotLight.Position;
          Renderer::DrawCube(lightSourceTransform, Colors::Blue);
        }

        for (auto pointLight : Renderer::Lights.GetSpotLights())
        {
          lightSourceTransform->Position = pointLight.Position;
          Renderer::DrawCube(lightSourceTransform, Colors::Yellow);
        }
      }
      Renderer::EndScene();

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
    case KeyCode::O:
    {
      Renderer::Lights.SetLightingModel(LightingModel::BlinnPhong);
      break;
    }
    case KeyCode::P:
    {
      Renderer::Lights.SetLightingModel(LightingModel::Phong);
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
    Renderer::Shutdown();
    Application::Shutdown();
  }
}