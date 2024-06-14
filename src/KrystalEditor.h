#include <Engine.h>
#include <Application.h>
#include <Misc/Controller.h>
#include <Graphics/Model.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/Transform.h>

#include <unordered_map>

namespace Krys
{
  class KrystalEditor : public Application
  {
    private:
      Ref<Camera> Camera;
      Ref<Controller> CameraController;

      std::unordered_map<string, Ref<VertexArray>> VertexArrays;
      std::unordered_map<string, Ref<VertexBuffer>> VertexBuffers;
      std::unordered_map<string, Ref<UniformBuffer>> UniformBuffers;
      std::unordered_map<string, Ref<IndexBuffer>> IndexBuffers;
      std::unordered_map<string, Ref<InstanceArrayBuffer>> InstanceArrayBuffers;
      std::unordered_map<string, Ref<Framebuffer>> Framebuffers;
      std::unordered_map<string, Ref<Shader>> Shaders;
      std::unordered_map<string, Ref<Texture2D>> Textures;
      std::unordered_map<string, Ref<TextureCubemap>> Cubemaps;
      std::unordered_map<string, Ref<Model>> Models;
      std::unordered_map<string, Ref<Material>> Materials;
      std::unordered_map<string, Ref<Transform>> Transforms;

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