#include "tools/debug/Debug.h"
#include <iomanip>
#include <iostream>
#include <variant>

#include "runtime/value/Value.h"

void disassembleChunk(const Chunk& chunk, const char* name)
{
  std::cout << name << ":\n";

  for (size_t offset = 0; offset < chunk.code.size();) {
    offset = disassembleInstruction(chunk, offset);
  }
}

static size_t simpleInstruction(const char* name, size_t offset)
{
  std::cout << name << "\n";
  return offset + 1;
}

static size_t constantInstruction(const char* name, const Chunk& chunk, size_t offset)
{
  Byte constantIdx = chunk.code[offset + 1];
  std::cout << std::setw(16) << std::setfill(' ') << std::left << name;
  std::visit(ValuePrinter{}, chunk.constants[constantIdx]);
  std::cout << "\n";
  return offset + 2;
}

static size_t jumpInstruction(const char* name, int sign, const Chunk& chunk, size_t offset)
{
  Byte jump = chunk.code[offset + 1];
  std::cout << name << " " << offset << " -> " << (offset + 2 + sign * jump) << "\n";
  return offset + 2;
}

size_t disassembleInstruction(const Chunk& chunk, size_t offset)
{
  std::cout << std::right << std::setw(4) << std::setfill('0') << offset << "   | ";

  OpCode op = static_cast<OpCode>(chunk.code[offset]);

  switch (op)
  {
    case OpCode::LOAD_CONSTANT:
      return constantInstruction("LOAD_CONSTANT", chunk, offset);

    case OpCode::ADD:
      return simpleInstruction("ADD", offset);

    case OpCode::SUB:
      return simpleInstruction("OP_SUBTRACT", offset);

    case OpCode::MUL:
      return simpleInstruction("OP_MULTIPLY", offset);

    case OpCode::DIV:
      return simpleInstruction("OP_DIVIDE", offset);

    case OpCode::PRINT:
      return simpleInstruction("PRINT", offset);

    case OpCode::RET:
      return simpleInstruction("RET", offset);

    case OpCode::POP:
      return simpleInstruction("POP", offset);

    case OpCode::GREATER:
      return simpleInstruction("GREATER", offset);

    case OpCode::GREATER_EQUAL:
      return simpleInstruction("GREATER_EQUAL", offset);

    case OpCode::LESS:
      return simpleInstruction("LESS", offset);

    case OpCode::LESS_EQUAL:
      return simpleInstruction("LESS_EQUAL", offset);

    case OpCode::NOT:
      return simpleInstruction("NOT", offset);

    case OpCode::EQUAL:
      return simpleInstruction("EQUAL", offset);

    case OpCode::NOT_EQUAL:
      return simpleInstruction("NOT_EQUAL", offset);

    case OpCode::TRUE:
      return simpleInstruction("TRUE", offset);

    case OpCode::FALSE:
      return simpleInstruction("FALSE", offset);

    case OpCode::NIL:
      return simpleInstruction("NIL", offset);

    case OpCode::DEFINE_GLOBAL:
      return constantInstruction("DEFINE_GLOBAL", chunk, offset);

    case OpCode::GET_GLOBAL:
      return constantInstruction("GET_GLOBAL", chunk, offset);

    case OpCode::SET_GLOBAL:
      return constantInstruction("SET_GLOBAL", chunk, offset);

    case OpCode::DEFINE_LOCAL:
      return simpleInstruction("DEFINE_LOCAL", offset);

    case OpCode::GET_LOCAL:
      return constantInstruction("GET_LOCAL", chunk, offset);

    case OpCode::SET_LOCAL:
      return constantInstruction("SET_LOCAL", chunk, offset);

    case OpCode::JMP_IF_FALSE:
      return jumpInstruction("JMP_IF_FALSE", 1, chunk, offset);

    case OpCode::JMP:
      return jumpInstruction("JMP", 1, chunk, offset);

    case OpCode::LOOP:
      return jumpInstruction("LOOP", -1, chunk, offset);
  }

  std::cout << "Unknown opcode " << static_cast<int>(op) << "\n";
  return offset + 1;
}
