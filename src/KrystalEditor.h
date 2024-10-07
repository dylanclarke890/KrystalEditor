#include <Engine.h>
#include <Application.h>
#include <Misc/Controller.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/Graphics.h>

namespace Krys
{
  class KrystalEditor : public Application
  {
    private:
      Ref<Camera> Camera;
      Ref<Controller> CameraController;

      Map<string, Ref<VertexArray>> VertexArrays;
      Map<string, Ref<VertexBuffer>> VertexBuffers;
      Map<string, Ref<UniformBuffer>> UniformBuffers;
      Map<string, Ref<IndexBuffer>> IndexBuffers;
      Map<string, Ref<InstanceArrayBuffer>> InstanceArrayBuffers;
      Map<string, Ref<Framebuffer>> Framebuffers;
      Map<string, Ref<Shader>> Shaders;
      Map<string, Ref<Texture2D>> Textures;
      Map<string, Ref<TextureCubemap>> Cubemaps;
      Map<string, Ref<Material>> Materials;
      Map<string, Ref<Transform>> Transforms;
      Map<string, Ref<SceneObject>> Objects;

    public:
      KrystalEditor();

      void Startup() override;
      void BeginFrame() override;
      void Update(float dt) override;
      void EndFrame() override;
      void Shutdown() override;

      void OnEvent(Event &event) override;

    private:
      bool OnKeyPressEvent(KeyPressedEvent &event);
      bool OnShutdownEvent(ShutdownEvent &event);
  };
}