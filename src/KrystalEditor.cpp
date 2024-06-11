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
  KrystalEditor::KrystalEditor()
      : Application("Krystal Editor", 1280, 720, 60.0f),
        Camera(nullptr), CameraController(nullptr), WireFrameMode(false),
        VertexArrays({}), UniformBuffers({}), VertexBuffers({}), IndexBuffers({}),
        InstanceArrayBuffers({}), Framebuffers({}), Textures({}), Models({}), Shaders({}),
        Materials({}), Transforms({})
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
    camera->SetPosition(Vec3(0.0f, 0.0f, 15.0f));
    Camera = camera;
    auto cameraController = CreateRef<PerspectiveCameraController>(camera);
    cameraController->SetSpeed(10.0f);
    CameraController = cameraController;

    Textures["crate"] = Context->CreateTexture2D("textures/crate.png");
    Materials["crate"] = CreateRef<Material>(Textures["crate"], Textures["crate-specular"]);
    Materials["crate"]->Shininess = 32.0f;
    Transforms["crate"] = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f), Vec3(0.0f));

    Textures["stage"] = Context->CreateTexture2D("textures/wood.png");
    Materials["stage"] = CreateRef<Material>(Textures["stage"]);
    Materials["stage"]->Shininess = 1.0f;
    Transforms["stage"] = CreateRef<Transform>(Vec3(0.0f, 0.5f, 0.0f), Vec3(20.0f, 20.0f, 0.1f));

    Renderer::SetSkybox({"cubemaps/space-skybox/right.png",
                         "cubemaps/space-skybox/left.png",
                         "cubemaps/space-skybox/top.png",
                         "cubemaps/space-skybox/bottom.png",
                         "cubemaps/space-skybox/front.png",
                         "cubemaps/space-skybox/back.png"});

#pragma region Light Setup
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
    samplePointLight.Enabled = true;
    samplePointLight.Intensity = 1.0f;                  // Full intensity
    samplePointLight.Position = Vec3(0.0f, 0.0f, 3.0f); // Position of the point light

    Renderer::Lights.AddPointLight(samplePointLight);

    SpotLight sampleSpotLight;
    sampleSpotLight.Ambient = Vec3(0.2f, 0.2f, 0.2f);  // Low ambient light
    sampleSpotLight.Diffuse = Vec3(0.8f, 0.8f, 0.8f);  // Moderate diffuse light
    sampleSpotLight.Specular = Vec3(1.0f, 1.0f, 1.0f); // Strong specular light
    sampleSpotLight.Constant = 1.0f;                   // Constant attenuation term
    sampleSpotLight.Linear = 0.09f;                    // Linear attenuation term
    sampleSpotLight.Quadratic = 0.032f;                // Quadratic attenuation term
    sampleSpotLight.Enabled = false;
    sampleSpotLight.Intensity = 1.0f;                            // Full intensity
    sampleSpotLight.Direction = Vec3(0.0f, -1.0f, 0.0f);         // Direction of the spotlight
    sampleSpotLight.Position = Vec3(0.0f, 0.0f, 0.0f);           // Position of the spotlight
    sampleSpotLight.InnerCutoff = glm::cos(glm::radians(12.5f)); // Inner cutoff angle (cosine of the angle)
    sampleSpotLight.OuterCutoff = glm::cos(glm::radians(17.5f)); // Outer cutoff angle (cosine of the angle)

    Renderer::Lights.AddSpotLight(sampleSpotLight);
#pragma endregion Light Setup

    Shaders["light-source"] = Context->CreateShader("shaders/lighting/light-source.vert", "shaders/lighting/light-source.frag");
    Transforms["light-source"] = CreateRef<Transform>(Vec3(0.0f), Vec3(0.3f));
  }

  void KrystalEditor::Update(float dt)
  {
    Window->BeginFrame();
    Input::BeginFrame();
    {
      KRYS_PERFORMANCE_TIMER("Frame")
      CameraController->OnUpdate(Time::GetDeltaSecs());
      Context->Clear(RenderBuffer::Color | RenderBuffer::Depth);

      Renderer::BeginScene(Camera, Shaders["light-test"]);
      {
        Renderer::DrawCube(Transforms["stage"], Materials["stage"]);
      }
      Renderer::EndScene();

      Renderer::BeginScene(Camera);
      {
        for (auto pointLight : Renderer::Lights.GetPointLights())
        {
          if (!pointLight.Enabled)
            continue;
          Transforms["light-source"]->Position = pointLight.Position;
          Renderer::DrawCube(Transforms["light-source"], Colors::Blue);
        }

        for (auto spotLight : Renderer::Lights.GetSpotLights())
        {
          if (!spotLight.Enabled)
            continue;
          Transforms["light-source"]->Position = spotLight.Position;
          Renderer::DrawCube(Transforms["light-source"], Colors::Yellow);
        }
      }
      Renderer::EndScene();
      Renderer::DrawSkybox(Camera);
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
    static bool useBlinn = false;
    static bool useSRGBFramebuffer = false;

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
      useBlinn = !useBlinn;
      Renderer::Lights.SetLightingModel(useBlinn ? LightingModel::BlinnPhong : LightingModel::Phong);
      break;
    }
    case KeyCode::G:
    {
      useSRGBFramebuffer = !useSRGBFramebuffer;
      Context->SetGammaCorrectionEnabled(useSRGBFramebuffer);
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