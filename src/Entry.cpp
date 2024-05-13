#include "KrystalEditor.h"

int main(int argc, char **argv)
{
  using namespace Krys;
  Unique<KrystalEditor> app = CreateUnique<KrystalEditor>();
  app->Run();
}