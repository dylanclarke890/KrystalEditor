#include "KrystalEditor.h"
#include <Graphics/Camera/Perspective.h>

namespace Krys
{
  KrystalEditor::KrystalEditor() noexcept
      : Application("Krystal Editor", 1280, 720, 60.0f),
        _camera(nullptr), _cameraController(nullptr)
  {
  }

  void KrystalEditor::Startup() noexcept
  {
    Application::Startup();

    _window->SetEventCallback(KRYS_BIND_EVENT_FN(KrystalEditor::OnEvent));

    _context->SetDepthTestingEnabled(true);
    _context->SetDepthTestFunc(DepthTestFunc::Less);
    _context->SetClearColor(Vec4(0.5f));

    auto camera = CreateRef<PerspectiveCamera>(_window->GetWidth(), _window->GetHeight(), 45.0f, 0.1f, 1000.0f);
    camera->SetPosition(Vec3(0.0f, 10.0f, 0.0f));
    camera->SetPitch(-90.0f);
    _camera = camera;
    auto cameraController = CreateRef<PerspectiveCameraController>(camera);
    cameraController->SetSpeed(10.0f);
    _cameraController = cameraController;
  }

  void KrystalEditor::Shutdown() noexcept
  {
    Application::Shutdown();
  }

  void KrystalEditor::BeginFrame() noexcept
  {
    Application::BeginFrame();
  }

  void KrystalEditor::Update(float dt) noexcept
  {
    Application::Update(dt);
  }

  void KrystalEditor::EndFrame() noexcept
  {
    Application::EndFrame();
  }

  void KrystalEditor::OnEvent(Event &event) noexcept
  {
    _cameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(KRYS_BIND_EVENT_FN(KrystalEditor::OnKeyPressEvent));

    Application::OnEvent(event);
  }

  bool KrystalEditor::OnKeyPressEvent(KeyPressedEvent &event) noexcept
  {
    static bool useBlinn = false;
    static bool useSRGBFramebuffer = false;
    static bool useWireframeMode = false;

    switch (event.Key)
    {
    case KeyCode::Space:
    {
      useWireframeMode = !useWireframeMode;
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
      break;
    }
    case KeyCode::G:
    {
      useSRGBFramebuffer = !useSRGBFramebuffer;
      _context->SetGammaCorrectionEnabled(useSRGBFramebuffer);
      break;
    }
    default:
      break;
    }

    return false;
  }

  bool KrystalEditor::OnShutdownEvent(ShutdownEvent &event) noexcept
  {
    Shutdown();
    return false;
  }
}