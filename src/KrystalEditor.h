#include <Engine.h>
#include <Application.h>
#include <Misc/Controller.h>
#include <Graphics/Model.h>
#include <Graphics/Framebuffer.h>

#include <unordered_map>

namespace Krys
{
  class KrystalEditor : public Application
  {
    private:
      Ref<Camera> Camera;
      Ref<Controller> CameraController;
      bool WireFrameMode;

      std::unordered_map<std::string, Ref<VertexArray>> VertexArrays;
      std::unordered_map<std::string, Ref<VertexBuffer>> VertexBuffers;
      std::unordered_map<std::string, Ref<UniformBuffer>> UniformBuffers;
      std::unordered_map<std::string, Ref<IndexBuffer>> IndexBuffers;
      std::unordered_map<std::string, Ref<InstanceArrayBuffer>> InstanceArrayBuffers;
      std::unordered_map<std::string, Ref<Framebuffer>> Framebuffers;
      std::unordered_map<std::string, Ref<Texture2D>> Textures;
      std::unordered_map<std::string, Ref<Model>> Models;
      std::unordered_map<std::string, Ref<Shader>> Shaders;

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