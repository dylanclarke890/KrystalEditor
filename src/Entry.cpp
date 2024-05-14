#include <Engine.h>
#include "KrystalEditor.h"

int main(int argc, char **argv)
{
  Krys::CreateEngine<Krys::KrystalEditor>()->Run();
}