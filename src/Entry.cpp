#include "KrystalEditor.hpp"
#include <Core/Application.hpp>

int main(int argc, char **argv)
{
  auto app = Krys::Application::Create<Krys::KrystalEditor>(argc, argv, 1'280, 800);
  app->Run();
}