#include <Engine.h>
#include <Application.h>
#include <Graphics/Camera/Camera.h>
#include <Misc/Controller.h>

namespace Krys
{
  class KrystalEditor : public Application
  {
    private:
      Ref<Camera> _camera;
      Ref<Controller> _cameraController;

    public:
      KrystalEditor() noexcept;

      void Startup() noexcept override;
      void Shutdown() noexcept override;

      void BeginFrame() noexcept override;
      void Update(float dt) noexcept override;
      void EndFrame() noexcept override;

      void OnEvent(Event &event) noexcept override;
    private:
      bool OnKeyPressEvent(KeyPressedEvent &event) noexcept;
      bool OnShutdownEvent(ShutdownEvent &event) noexcept;
  };
}