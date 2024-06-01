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
    TestShader->Load("shaders/renderer-2d.vert", "shaders/scene-texture.frag");
    TestShader->Link();

    int samplers[REN2D_MAX_TEXTURE_SLOTS]{};
    for (uint32_t i = 0; i < REN2D_MAX_TEXTURE_SLOTS; i++)
      samplers[i] = i;
    TestShader->SetUniform("u_Textures", samplers, REN2D_MAX_TEXTURE_SLOTS);

    TestFramebuffer = Context->CreateFramebuffer();
    TestFramebuffer->AddColorAttachment(Window->GetWidth(), Window->GetHeight());
    TestFramebuffer->AddDepthStencilAttachment(Window->GetWidth(), Window->GetHeight());

    float skyboxVertices[] = {
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
    uint32 skyboxIndices[36] = {};
    for (uint i = 0; i < 36; i++)
      skyboxIndices[i] = i;
    SkyboxVAO = Context->CreateVertexArray();
    SkyboxVBO = Context->CreateVertexBuffer(skyboxVertices, sizeof(skyboxVertices));
    SkyboxVBO->SetLayout(BufferLayout(sizeof(skyboxVertices), {{ShaderDataType::Float3, "a_Position"}}));
    SkyboxEBO = Context->CreateIndexBuffer(skyboxIndices, 36);

    SkyboxVAO->AddVertexBuffer(SkyboxVBO);
    SkyboxVAO->SetIndexBuffer(SkyboxEBO);

    SkyboxShader = Context->CreateShader();
    SkyboxShader->Load("shaders/skybox.vert", "shaders/skybox.frag");
    SkyboxShader->Link();
  }

  void KrystalEditor::Update(float dt)
  {
    static std::vector<std::string> faces = {
        "cubemaps/skybox/right.jpg",
        "cubemaps/skybox/left.jpg",
        "cubemaps/skybox/top.jpg",
        "cubemaps/skybox/bottom.jpg",
        "cubemaps/skybox/front.jpg",
        "cubemaps/skybox/back.jpg"};
    static auto skyboxTexture = Context->CreateTextureCubemap(faces);

    static auto objectTexture = Context->CreateTexture2D("textures/crate.png");
    static auto windowTexture = Context->CreateTexture2D("textures/blending_transparent_window.png");

    static auto stageTransform = CreateRef<Transform>(Vec3(0.0f, -0.25f, 0.0f), Vec3(15.0f, 0.25f, 15.0f));
    static auto objectTransform = CreateRef<Transform>(Vec3(0.0f, 0.54f, 0.0f), Vec3(1.0f));

    static auto objectMaterial = CreateRef<Material>(objectTexture);
    static auto windowMaterial = CreateRef<Material>(windowTexture);

    Window->BeginFrame();
    Input::BeginFrame();
    {
      CameraController->OnUpdate(Time::GetDeltaSecs());

      Context->Clear(ClearFlags::Color | ClearFlags::Depth);

      Renderer2D::BeginScene(Camera);
      {
        Renderer2D::DrawCube(stageTransform, Colors::Gray50);
        objectTransform->Position.z = 0.0f;
        Renderer2D::DrawCube(objectTransform, objectMaterial);
        objectTransform->Position.z = 2.0f;
        objectTransform->Position.x = 0.5f;
        Renderer2D::DrawQuad(objectTransform, windowMaterial);
        objectTransform->Position.x = 0.0f;
        objectTransform->Position.z = 1.0f;
        Renderer2D::DrawQuad(objectTransform, windowMaterial);
      }
      Renderer2D::EndScene();

      Context->SetDepthTestFunc(DepthTestFunc::EqualOrLess);
      {
        SkyboxShader->Bind();
        auto view = Mat4(Mat3(Camera->GetView()));
        auto viewProjection = Camera->GetProjection() * view;
        SkyboxShader->SetUniform("u_ViewProjection", viewProjection);
        SkyboxVAO->Bind();
        skyboxTexture->Bind();
        Renderer2D::DrawIndexed(SkyboxEBO->Size());
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