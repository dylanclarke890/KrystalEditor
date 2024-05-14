#include <Engine.h>
#include <Application.h>

namespace Krys
{
  class KrystalEditor : public Application
  {
    private:
      Ref<Camera> Camera;
      bool WireFrameMode;

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