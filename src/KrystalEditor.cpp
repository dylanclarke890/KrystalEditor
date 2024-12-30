#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <IO/Compression/Huffman.hpp>
#include <IO/Compression/RLE.hpp>
#include <IO/DataFlow.hpp>
#include <IO/IO.hpp>
#include <IO/Readers/BinaryFileReader.hpp>
#include <IO/Readers/MemoryReader.hpp>
#include <IO/Writers/BinaryFileWriter.hpp>

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

    using data_src_t = IO::MemoryReader<Endian::Type::System, Endian::Type::System>;
    using data_dst_t = IO::BinaryFileWriter<Endian::Type::System, Endian::Type::System>;
    using data_flow_t = IO::DataFlow<data_src_t, data_dst_t>;

    using identity_t = IO::Stage::Identity<List<byte>>;

    List<byte> sampleData = {byte {72}, byte {101}, byte {108}, byte {108}, byte {111}, byte {32},
                             byte {87}, byte {111}, byte {114}, byte {108}, byte {100}};
    auto input = data_src_t(sampleData);
    auto output = data_dst_t("./compress/decoded.txt");

    auto pipeline = data_flow_t(&input, &output) | identity_t {};
    pipeline.Execute();

    _running = false;
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}