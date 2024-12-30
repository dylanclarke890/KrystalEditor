#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <IO/IO.hpp>
#include <IO/Readers/BinaryFileReader.hpp>
#include <IO/Readers/MemoryReader.hpp>
#include <IO/Writers/BinaryFileWriter.hpp>
#include <Scripting/Parser.hpp>

namespace Krys
{
  KrystalEditor::KrystalEditor(Unique<ApplicationContext> context, uint32 width, uint32 height) noexcept
      : Application(std::move(context), width, height)
  {
    _context->GetEventManager()->RegisterHandler<KeyboardEvent>(
      [](const KeyboardEvent &event)
      {
        KRYS_LOG("key pressed %d", event.GetKey());
        return false;
      });

    using data_src_t = IO::BinaryFileReader<Endian::Type::System, Endian::Type::System>;
    using data_dst_t = IO::BinaryFileWriter<Endian::Type::System, Endian::Type::System>;

    data_src_t src {"./compress/input.txt"};
    data_dst_t dst {"./compress/decoded.txt"};
    Lang::Parser parser(src, dst);
    parser.Parse();

    _running = false;
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}