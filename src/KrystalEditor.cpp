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
    Context->SetStencilTestingEnabled(true);
    Context->SetBlendingEnabled(true);

    Context->SetDepthTestFunc(DepthTestFunc::Less);
    Context->SetBlendFunc(BlendFactor::SourceAlpha, BlendFactor::OneMinusSourceAlpha);

    auto camera = CreateRef<PerspectiveCamera>(Window->GetWidth(), Window->GetHeight(), 45.0f);
    Camera = camera;
    CameraController = CreateRef<PerspectiveCameraController>(camera);

    camera->SetYaw(25.0f);
    camera->SetPitch(-40.0f);
    camera->SetPosition(Vec3(-1.0f, 7.0f, 10.0f));

    //   objectShader->SetUniform("u_DirectionalLight.Enabled", true);
    //   objectShader->SetUniform("u_DirectionalLight.Direction", Vec3(0.0f, -1.0f, 0.0f));
    //   objectShader->SetUniform("u_DirectionalLight.Ambient", Vec3(0.5f));
    //   objectShader->SetUniform("u_DirectionalLight.Diffuse", Vec3(0.5f));
    //   objectShader->SetUniform("u_DirectionalLight.Specular", Vec3(1.0f));

    //   objectShader->SetUniform("u_PointLight.Enabled", true);
    //   objectShader->SetUniform("u_PointLight.Ambient", Vec3(0.5f));
    //   objectShader->SetUniform("u_PointLight.Diffuse", Vec3(0.5f));
    //   objectShader->SetUniform("u_PointLight.Specular", Vec3(1.0f));
    //   objectShader->SetUniform("u_PointLight.Constant", 1.0f);
    //   objectShader->SetUniform("u_PointLight.Linear", 0.09f);
    //   objectShader->SetUniform("u_PointLight.Quadratic", 0.032f);

    //   objectShader->SetUniform("u_SpotLight.Enabled", true);
    //   objectShader->SetUniform("u_SpotLight.Ambient", Vec3(0.1f));
    //   objectShader->SetUniform("u_SpotLight.Diffuse", Vec3(0.8f));
    //   objectShader->SetUniform("u_SpotLight.Specular", Vec3(1.0f));
    //   objectShader->SetUniform("u_SpotLight.Constant", 1.0f);
    //   objectShader->SetUniform("u_SpotLight.Linear", 0.09f);
    //   objectShader->SetUniform("u_SpotLight.Quadratic", 0.032f);
    //   objectShader->SetUniform("u_SpotLight.InnerCutoff", glm::cos(glm::radians(12.5f)));
    //   objectShader->SetUniform("u_SpotLight.OuterCutoff", glm::cos(glm::radians(17.5f)));

    TestShader = Context->CreateShader();
    TestShader->Load("shaders/light-source.vert", "shaders/light-source.frag");
    TestShader->Link();

    TestFramebuffer = Context->CreateFramebuffer();
    TestFramebuffer->AddColorAttachment(Window->GetWidth(), Window->GetHeight());
    TestFramebuffer->AddDepthStencilAttachment(Window->GetWidth(), Window->GetHeight());
  }

  void KrystalEditor::Update(float dt)
  {
    static auto objectTexture = Context->CreateTexture2D("textures/crate.png");
    static auto objectSpecularTexture = Context->CreateTexture2D("textures/crate-spec.png");
    static auto objectEmissionTexture = Context->CreateTexture2D("textures/crate-emission.png");
    static auto grassTexture = Context->CreateTexture2D("textures/grass.png");
    grassTexture->SetTextureWrapModes(TextureWrapMode::ClampToEdge, TextureWrapMode::ClampToEdge);
    static auto windowTexture = Context->CreateTexture2D("textures/blending_transparent_window.png");

    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f));
    static auto frameBufferTargetTransform = CreateRef<Transform>(Vec3(-1.0f, 7.0f, 9.0f), Vec3(1.0f));
    static auto lightSourceTransform = CreateRef<Transform>(Vec3(0.0f, 1.0f, 0.0f), Vec3(0.2f));

    static auto objectMaterial = CreateRef<Material>(windowTexture);
    static auto frameBufferMaterial = CreateRef<Material>(TestFramebuffer->GetColorAttachment());

    Window->BeginFrame();
    Input::BeginFrame();
    {
      // KRYS_PERFORMANCE_TIMER("Update");
      TestFramebuffer->Bind();
      Context->SetDepthTestingEnabled(true);
      Context->Clear(ClearFlags::Color | ClearFlags::Depth);

      static auto lightMoveSpeed = 0.01f;
      if (Input::IsKeyPressed(KeyCode::LeftArrow))
        lightSourceTransform->Position.x -= lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::RightArrow))
        lightSourceTransform->Position.x += lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::UpArrow))
        lightSourceTransform->Position.y += lightMoveSpeed * dt;
      if (Input::IsKeyPressed(KeyCode::DownArrow))
        lightSourceTransform->Position.y -= lightMoveSpeed * dt;

      CameraController->OnUpdate(Time::GetDeltaSecs());

      Renderer2D::BeginScene(Camera);
      {
        Renderer2D::DrawCube(stageTransform, Colors::Gray50);
        // for (float i = 0.0f; i < 20.0f; i++)
        // {
        //   objectTransform->Position.x = (i * 0.75f) - ((stageTransform->Size.x - objectTransform->Size.x) / 2.0f);
        //   for (float j = 0.0f; j < 20.0f; j++)
        //   {
        //     objectTransform->Position.z = (j * 0.75f) - ((stageTransform->Size.z - objectTransform->Size.z) / 2.0f);
        //     Renderer2D::DrawQuad(objectTransform, objectMaterial);
        //   }
        // }

        objectTransform->Position.z = 0.0f;
        Renderer2D::DrawCube(objectTransform, Colors::Coral);
        objectTransform->Position.z = 2.0f;
        objectTransform->Position.x = 0.5f;
        Renderer2D::DrawQuad(objectTransform, objectMaterial);
        objectTransform->Position.x = 0.0f;
        objectTransform->Position.z = 1.0f;
        Renderer2D::DrawQuad(objectTransform, objectMaterial);
      }
      Renderer2D::EndScene();

      TestFramebuffer->Unbind();
      Context->Clear(ClearFlags::Color);
      Context->SetDepthTestingEnabled(false);

      Renderer2D::BeginScene(Camera);
      {
        frameBufferTargetTransform->Size = Vec3(1.2f);
        Renderer2D::DrawQuad(frameBufferTargetTransform, Colors::Red);
        frameBufferTargetTransform->Size = Vec3(1.0f);
        Renderer2D::DrawQuad(frameBufferTargetTransform, frameBufferMaterial);
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