#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <IO/Compression/Huffman.hpp>
#include <IO/DataFlow.hpp>
#include <IO/IO.hpp>
#include <IO/Readers/BinaryFileReader.hpp>
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

    using namespace IO::Stage;
    using data_src_t = IO::BinaryFileReader<Endian::Type::System, Endian::Type::System>;
    using data_dst_t = IO::BinaryFileWriter<Endian::Type::System, Endian::Type::System>;
    using data_flow_t = IO::DataFlow<data_src_t, data_dst_t>;

    using identity_t = Identity<List<byte>, List<byte>>;
    using encoder_t = HuffmanEncoder<uint16, Endian::Type::System, Endian::Type::System>;
    using decoder_t = HuffmanDecoder<uint16, Endian::Type::System, Endian::Type::System>;

    data_src_t input("compress/input.txt");
    data_dst_t output("compress/decoded.txt");

    auto pipeline = data_flow_t(&input, &output) | encoder_t {} | decoder_t {};
    pipeline.Execute();

    _running = false;
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}