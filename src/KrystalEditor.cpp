#include "KrystalEditor.hpp"
#include <Core/Debug/Macros.hpp>
#include <Core/Events/Input/KeyboardEvent.hpp>
#include <IO/BinaryFileReader.hpp>
#include <IO/BinaryFileWriter.hpp>
#include <IO/Compression/HuffmanDecoder.hpp>
#include <IO/Compression/HuffmanEncoder.hpp>
#include <IO/Compression/HuffmanTree.hpp>
#include <IO/IO.hpp>

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

    using encoder_t = IO::HuffmanEncoder<uint16, Endian::Type::System>;
    using decoder_t = IO::HuffmanDecoder<uint16, Endian::Type::System>;

    encoder_t encoder("./input.txt", "./encoded.txt");
    encoder.Encode();
    decoder_t decoder("./encoded.txt", "./decoded.txt", encoder.GetTree());
    decoder.Decode();

    _running = false;
  }

  void KrystalEditor::Update(float) noexcept
  {
  }
}