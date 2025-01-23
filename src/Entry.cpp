#include "KrystalEditor.hpp"
#include <Core/Application.hpp>

int main(int argc, char **argv)
{
  using namespace Krys;

  ApplicationSettings settings;
  settings.VSync = false;
  settings.Width = 1'920;
  settings.Height = 1'080;

  auto app = Application::Create<KrystalEditor>(argc, argv, settings);
  app->Run();
}