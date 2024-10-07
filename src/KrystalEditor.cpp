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
        Materials({}), Transforms({})
  {
  }

  void KrystalEditor::Startup()
  {
    Application::Startup();

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

#pragma region Resource Creation

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

    Materials["crate"] = Context->CreateMaterial();
    Materials["crate"]->DiffuseMap = Textures["crate"];
    Materials["crate"]->SpecularMap = Textures["crate-specular"];
    Materials["crate"]->Shininess = 32.0f;

    Materials["stage"] = Context->CreateMaterial();
    Materials["stage"]->DiffuseMap = Textures["brickwall"];
    Materials["stage"]->NormalMap = Textures["brickwall-normal"];
    Materials["stage"]->Shininess = 1.0f;

    Materials["red-bricks"] = Context->CreateMaterial();
    Materials["red-bricks"]->DiffuseMap = Textures["red-bricks"];
    Materials["red-bricks"]->NormalMap = Textures["red-bricks-normal"];
    Materials["red-bricks"]->DisplacementMap = Textures["red-bricks-displacement"];

    Materials["toy-box"] = Context->CreateMaterial();
    Materials["toy-box"]->DiffuseMap = Textures["wood"];
    // Materials["toy-box"]->Normal = Textures["toy-box-normal"];
    // Materials["toy-box"]->Displacement = Textures["toy-box-displacement"];

    Transforms["origin"] = CreateRef<Transform>(Vec3(0.0f), Vec3(1.0f));
    Transforms["stage"] = CreateRef<Transform>(Vec3(0.0f, -10.0f, 0.0f), Vec3(20.0f, 1.0f, 20.0f));
    Transforms["crate"] = CreateRef<Transform>(Vec3(0.0f, -6.0f, 0.0f), Vec3(1.0f), Vec3(0.0f));
    Transforms["square"] = CreateRef<Transform>(Vec3(0.0f, -4.0f, 0.0f), Vec3(5.0f, 1.0f, 5.0f), Vec3(0.0f));
    // Renderer::SetSkybox({"cubemaps/space-skybox/right.png",
    //                      "cubemaps/space-skybox/left.png",
    //                      "cubemaps/space-skybox/top.png",
    //                      "cubemaps/space-skybox/bottom.png",
    //                      "cubemaps/space-skybox/front.png",
    //                      "cubemaps/space-skybox/back.png"});
#pragma endregion Resource Creation

    Objects["Mannequin"] = Context->CreateSceneObject("models/Mannequin.glb")[0];
  }

  void KrystalEditor::Update(float dt)
  {
    KRYS_PERFORMANCE_TIMER("Frame");
    CameraController->OnUpdate(dt);
    Context->Clear(RenderBuffer::Color | RenderBuffer::Depth);

    Renderer::Begin(Camera);
    Renderer::Draw(Objects["Mannequin"]);
    Renderer::End();
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
      Renderer::SetLightingModel(useBlinn ? LightingModelType::BlinnPhong : LightingModelType::Phong);
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
    Application::Shutdown();
  }
}