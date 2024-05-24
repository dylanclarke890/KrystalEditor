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

    Context->SetDepthTestingEnabled(true);
    Context->SetDepthTestFunc(DepthTestFunc::Less);

    auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f);
    Camera = camera;
    CameraController = CreateRef<PerspectiveCameraController>(camera);

    camera->SetYaw(25.0f);
    camera->SetPitch(-40.0f);
    camera->SetPosition(Vec3(-1.0f, 7.0f, 10.0f));

    auto objectShader = Renderer2D::GetObjectShader();

    objectShader->SetUniform("u_DirectionalLight.Enabled", true);
    objectShader->SetUniform("u_DirectionalLight.Direction", Vec3(0.0f, -1.0f, 0.0f));
    objectShader->SetUniform("u_DirectionalLight.Ambient", Vec3(0.5f));
    objectShader->SetUniform("u_DirectionalLight.Diffuse", Vec3(0.5f));
    objectShader->SetUniform("u_DirectionalLight.Specular", Vec3(1.0f));

    objectShader->SetUniform("u_PointLight.Enabled", true);
    objectShader->SetUniform("u_PointLight.Ambient", Vec3(0.5f));
    objectShader->SetUniform("u_PointLight.Diffuse", Vec3(0.5f));
    objectShader->SetUniform("u_PointLight.Specular", Vec3(1.0f));
    objectShader->SetUniform("u_PointLight.Constant", 1.0f);
    objectShader->SetUniform("u_PointLight.Linear", 0.09f);
    objectShader->SetUniform("u_PointLight.Quadratic", 0.032f);

    objectShader->SetUniform("u_SpotLight.Enabled", true);
    objectShader->SetUniform("u_SpotLight.Ambient", Vec3(0.1f));
    objectShader->SetUniform("u_SpotLight.Diffuse", Vec3(0.8f));
    objectShader->SetUniform("u_SpotLight.Specular", Vec3(1.0f));
    objectShader->SetUniform("u_SpotLight.Constant", 1.0f);
    objectShader->SetUniform("u_SpotLight.Linear", 0.09f);
    objectShader->SetUniform("u_SpotLight.Quadratic", 0.032f);
    objectShader->SetUniform("u_SpotLight.InnerCutoff", glm::cos(glm::radians(12.5f)));
    objectShader->SetUniform("u_SpotLight.OuterCutoff", glm::cos(glm::radians(17.5f)));
  }

  void KrystalEditor::Update(float dt)
  {
    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, 0.0f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto objectTransform = CreateRef<Transform>(Vec3(2.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
    static auto objectTexture = Context->CreateTexture2D("textures/crate.png");
    static auto objectSpecularTexture = Context->CreateTexture2D("textures/crate-spec.png");
    static auto objectEmissionTexture = Context->CreateTexture2D("textures/crate-emission.png");
    static auto objectMaterial = CreateRef<Mesh>(objectTexture, objectSpecularTexture);
    objectMaterial->Shininess = 2.0f;

    static auto lightSourceTransform = CreateRef<Transform>(Vec3(0.0f, 1.0f, 0.0f), Vec3(0.2f));
    static auto lightMoveSpeed = 0.01f;

    Window->BeginFrame();
    Input::BeginFrame();
    {
      KRYS_PERFORMANCE_TIMER("Update");

      if (Input::IsKeyPressed(KeyCode::LeftArrow))
        lightSourceTransform->Position.x -= lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::RightArrow))
        lightSourceTransform->Position.x += lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::UpArrow))
        lightSourceTransform->Position.y += lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::DownArrow))
        lightSourceTransform->Position.y -= lightMoveSpeed * dt;

      auto objectShader = Renderer2D::GetObjectShader();
      objectShader->SetUniform("u_PointLight.Position", lightSourceTransform->Position);

      auto camera = reinterpret_cast<PerspectiveCamera *>(Camera.get());
      objectShader->SetUniform("u_SpotLight.Position", camera->GetPosition());
      objectShader->SetUniform("u_SpotLight.Direction", camera->GetFront());

      CameraController->OnUpdate(Time::GetDeltaSecs());
      Renderer2D::BeginScene(Camera);
      {
        Context->Clear(ClearFlags::Color | ClearFlags::Depth);

        Renderer2D::DrawCube(stageTransform, Colors::Gray50);
        Renderer2D::DrawCube(objectTransform, objectMaterial);

        Renderer2D::DrawLightSourceCube(lightSourceTransform);
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