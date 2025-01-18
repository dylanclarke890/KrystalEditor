#include "KrystalEditor.hpp"
#include <Core/Application.hpp>

int main(int argc, char **argv)
{
  using namespace Krys;

  ApplicationSettings settings;
  settings.VSync = false;

  auto app = Application::Create<KrystalEditor>(argc, argv, settings);
  app->Run();
}