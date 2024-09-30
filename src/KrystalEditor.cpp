#include <Graphics/Renderer/Renderer.h>
#include <Graphics/Camera/Orthographic.h>
#include <Graphics/Camera/Perspective.h>
#include <Graphics/Colors.h>

#include <IO/IO.h>

#include <Misc/Performance.h>
#include <Misc/Time.h>
#include <Misc/Chrono.h>
#include <Misc/Random.h>

#include "KrystalEditor.h"

namespace Krys
{
  KrystalEditor::KrystalEditor()
      : Application("Krystal Editor", 1280, 720, 60.0f),
        Camera(nullptr), CameraController(nullptr),
        VertexArrays({}), UniformBuffers({}), VertexBuffers({}), IndexBuffers({}),
        InstanceArrayBuffers({}), Framebuffers({}), Shaders({}), Textures({}), Cubemaps({}),
        Models({}), Materials({}), Transforms({})
  {
  }

  void KrystalEditor::Startup()
  {
    Application::Startup();

    Renderer::Init(Window, Context);
    Renderer::SetPostProcessingEnabled(true);

    Random::Init();

    Window->SetEventCallback(KRYS_BIND_EVENT_FN(KrystalEditor::OnEvent));

    Context->SetDepthTestingEnabled(true);
    Context->SetDepthTestFunc(DepthTestFunc::Less);
    Context->SetClearColor(Vec4(0.5f));

    auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f, 0.1f, 1000.0f);
    camera->SetPosition(Vec3(0.0f, 10.0f, 0.0f));
    camera->SetPitch(-90.0f);
    Camera = camera;
    auto cameraController = CreateRef<PerspectiveCameraController>(camera);
    cameraController->SetSpeed(10.0f);
    CameraController = cameraController;

    Textures["crate"] = Context->CreateTexture2D("textures/crate.png");
    Textures["crate-specular"] = Context->CreateTexture2D("textures/crate-specular.png");

    Textures["wood"] = Context->CreateTexture2D("textures/wood.png");

    Textures["brickwall"] = Context->CreateTexture2D("textures/brickwall.jpg");
    Textures["brickwall-normal"] = Context->CreateTexture2D("textures/brickwall-normal.jpg");

    Textures["red-bricks"] = Context->CreateTexture2D("textures/red-bricks.jpg");
    Textures["red-bricks-normal"] = Context->CreateTexture2D("textures/red-bricks-normal.jpg");
    Textures["red-bricks-displacement"] = Context->CreateTexture2D("textures/red-bricks-displacement.jpg");

    Textures["toy-box-normal"] = Context->CreateTexture2D("textures/toy-box-normal.png");
    Textures["toy-box-displacement"] = Context->CreateTexture2D("textures/toy-box-displacement.png");

    Materials["crate"] = CreateRef<Material>(Textures["crate"]);
    Materials["crate"]->Specular = Textures["crate-specular"];
    Materials["crate"]->Shininess = 32.0f;

    Materials["stage"] = CreateRef<Material>(Textures["brickwall"]);
    Materials["stage"]->Normal = Textures["brickwall-normal"];
    Materials["stage"]->Shininess = 1.0f;

    Materials["red-bricks"] = CreateRef<Material>(Textures["red-bricks"]);
    Materials["red-bricks"]->Normal = Textures["red-bricks-normal"];
    Materials["red-bricks"]->Displacement = Textures["red-bricks-displacement"];

    Materials["toy-box"] = CreateRef<Material>(Textures["wood"]);
    // Materials["toy-box"]->Normal = Textures["toy-box-normal"];
    // Materials["toy-box"]->Displacement = Textures["toy-box-displacement"];

    Transforms["stage"] = CreateRef<Transform>(Vec3(0.0f, -10.0f, 0.0f), Vec3(20.0f, 1.0f, 20.0f));
    Transforms["crate"] = CreateRef<Transform>(Vec3(0.0f, -6.0f, 0.0f), Vec3(1.0f), Vec3(0.0f));
    Transforms["square"] = CreateRef<Transform>(Vec3(0.0f, -4.0f, 0.0f), Vec3(5.0f, 1.0f, 5.0f), Vec3(0.0f));
    // Renderer::SetSkybox({"cubemaps/space-skybox/right.png",
    //                      "cubemaps/space-skybox/left.png",
    //                      "cubemaps/space-skybox/top.png",
    //                      "cubemaps/space-skybox/bottom.png",
    //                      "cubemaps/space-skybox/front.png",
    //                      "cubemaps/space-skybox/back.png"});

#pragma region Light Setup
    Renderer::Lights.EnableLighting();
    Renderer::Lights.EnableShadows();

    DirectionalLight sampleDirectionalLight{};
    sampleDirectionalLight.Ambient = Vec3(0.2f);
    sampleDirectionalLight.Diffuse = Vec3(0.5f);
    sampleDirectionalLight.Specular = Vec3(1.0f);
    sampleDirectionalLight.Enabled = true;
    sampleDirectionalLight.Intensity = 1.0f;
    sampleDirectionalLight.Direction = Vec3(0.0f, -1.0f, 0.0f);

    Renderer::Lights.AddLight(sampleDirectionalLight, {true});

    sampleDirectionalLight = DirectionalLight{};
    sampleDirectionalLight.Ambient = Vec3(0.2f);
    sampleDirectionalLight.Diffuse = Vec3(0.3f);
    sampleDirectionalLight.Specular = Vec3(1.0f);
    sampleDirectionalLight.Enabled = true;
    sampleDirectionalLight.Intensity = 1.0f;
    sampleDirectionalLight.Direction = Vec3(0.1f, -1.0f, 0.0f);

    Renderer::Lights.AddLight(sampleDirectionalLight, {true});

    PointLight samplePointLight{};
    samplePointLight.Ambient = Vec3(0.0f);
    samplePointLight.Diffuse = Vec3(0.8f, 0.8f, 0.8f);
    samplePointLight.Specular = Vec3(1.0f);
    samplePointLight.Constant = 1.0f;
    samplePointLight.Linear = 0.09f;
    samplePointLight.Quadratic = 0.032f;
    samplePointLight.Enabled = true;
    samplePointLight.Intensity = 1.0f;
    samplePointLight.FarPlane = 25.0f;
    samplePointLight.Position = Vec3(3.0f, 0.0f, 0.0f);

    Renderer::Lights.AddLight(samplePointLight, {true});

    samplePointLight = PointLight{};
    samplePointLight.Ambient = Vec3(0.0f, 0.0f, 0.0f);
    samplePointLight.Diffuse = Vec3(0.0f, 0.5f, 0.0f);
    samplePointLight.Specular = Vec3(0.0f, 1.0f, 0.0f);
    samplePointLight.Constant = 1.0f;
    samplePointLight.Linear = 0.09f;
    samplePointLight.Quadratic = 0.032f;
    samplePointLight.Enabled = true;
    samplePointLight.Intensity = 1.0f;
    samplePointLight.FarPlane = 25.0f;
    samplePointLight.Position = Vec3(-3.0f, -1.0f, 0.0f);

    Renderer::Lights.AddLight(samplePointLight, {true});

    SpotLight sampleSpotLight{};
    sampleSpotLight.Ambient = Vec3(0.0f);
    sampleSpotLight.Diffuse = Vec3(0.4f);
    sampleSpotLight.Specular = Vec3(1.0f);
    sampleSpotLight.Constant = 1.0f;
    sampleSpotLight.Linear = 0.09f;
    sampleSpotLight.Quadratic = 0.032f;
    sampleSpotLight.Enabled = true;
    sampleSpotLight.Intensity = 1.0f;
    sampleSpotLight.Direction = Vec3(0.3f, -1.0f, 0.0f);
    sampleSpotLight.Position = Vec3(-2.0f, 1.0f, 0.0f);
    sampleSpotLight.InnerCutoff = glm::radians(12.5f);
    sampleSpotLight.OuterCutoff = glm::radians(17.5f);

    Renderer::Lights.AddLight(sampleSpotLight, {true});

    sampleSpotLight = SpotLight{};
    sampleSpotLight.Ambient = Vec3(0.0f);
    sampleSpotLight.Diffuse = Vec3(0.5f);
    sampleSpotLight.Specular = Vec3(1.0f);
    sampleSpotLight.Constant = 1.0f;
    sampleSpotLight.Linear = 0.09f;
    sampleSpotLight.Quadratic = 0.032f;
    sampleSpotLight.Enabled = true;
    sampleSpotLight.Intensity = 1.0f;
    sampleSpotLight.Direction = Vec3(-0.3f, -1.0f, 0.0f);
    sampleSpotLight.Position = Vec3(2.0f, -1.0f, 0.0f);
    sampleSpotLight.InnerCutoff = glm::radians(12.5f);
    sampleSpotLight.OuterCutoff = glm::radians(17.5f);

    Renderer::Lights.AddLight(sampleSpotLight, {true});
#pragma endregion Light Setup
  }

  void KrystalEditor::Update(float dt)
  {
    KRYS_PERFORMANCE_TIMER("Frame");

    CameraController->OnUpdate(dt);
    Context->Clear(RenderBuffer::Color | RenderBuffer::Depth);

    const float speed = 20.0f;
    auto crateTransform = Transforms["crate"];

    if (Input::IsKeyPressed(KeyCode::UpArrow))
    {
      crateTransform->Position.y += speed * dt;
      Transforms["stage"]->Position.y += speed * dt;
    }

    if (Input::IsKeyPressed(KeyCode::DownArrow))
    {
      crateTransform->Position.y -= speed * dt;
      Transforms["stage"]->Position.y -= speed * dt;
    }

    crateTransform->Rotation.x += speed * dt;
    crateTransform->Rotation.y += speed * dt;

    Renderer::BeginScene(Camera);
    {
      Renderer::DrawCube(crateTransform, Materials["crate"]);
      Renderer::DrawCube(Transforms["stage"], Materials["toy-box"]);
    }
    Renderer::EndScene();
  }

  void KrystalEditor::OnEvent(Event &event)
  {
    CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(KRYS_BIND_EVENT_FN(KrystalEditor::OnKeyPressEvent));

    Application::OnEvent(event);
  }

  void Krys::KrystalEditor::BeginFrame()
  {
    Application::BeginFrame();
  }

  void Krys::KrystalEditor::EndFrame()
  {
    Application::EndFrame();
  }

  bool KrystalEditor::OnKeyPressEvent(KeyPressedEvent &event)
  {
    static bool useBlinn = false;
    static bool useSRGBFramebuffer = false;
    static bool useWireframeMode = false;

    switch (event.Key)
    {
    case KeyCode::Space:
    {
      useWireframeMode = !useWireframeMode;
      Renderer::SetWireFrameModeEnabled(useWireframeMode);
      break;
    }
    case KeyCode::Escape:
    {
      Stop();
      break;
    }
    case KeyCode::P:
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