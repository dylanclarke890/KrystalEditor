#include <Engine.h>
#include <Application.h>
#include <Misc/Controller.h>
#include <Graphics/Model.h>
#include <Graphics/Framebuffer.h>

namespace Krys
{
  class KrystalEditor : public Application
  {
    private:
      Ref<Camera> Camera;
      Ref<Controller> CameraController;
      bool WireFrameMode;
      Ref<Model> TestModel;
      Ref<Shader> TestShader;
      Ref<Framebuffer> TestFramebuffer;

      Ref<Shader> SkyboxShader;
      Ref<VertexArray> SkyboxVAO;
      Ref<VertexBuffer> SkyboxVBO;
      Ref<IndexBuffer> SkyboxEBO;

      Ref<Shader> EnvironmentMappingShader;

    public:
      KrystalEditor();

      void Startup() override;
      void Update(float dt) override;
      void Shutdown() override;

      void OnEvent(Event &event) override;

    private:
      bool OnKeyPressEvent(KeyPressedEvent &event);
      bool OnShutdownEvent(ShutdownEvent &event);
  };
}