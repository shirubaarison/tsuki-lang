#include "VM/Debug.h"
#include <iomanip>
#include <iostream>
#include <variant>

#include "Value.h"

void disassembleChunk(const std::vector<Instruction>& code, const char* name) {
  std::cout << name << ":\n";

  for (size_t offset = 0; offset < code.size(); ++offset) {
    disassembleInstruction(code[offset], offset);
  }
}

void disassembleInstruction(const Instruction& instr, size_t offset) {
  std::cout << std::setw(4) << std::setfill('0') << offset << " ";

  switch (instr.op) {
    case OpCode::OP_CONSTANT:
      std::cout << std::setw(16) << std::setfill(' ') << std::left
        << "OP_CONSTANT";
      std::visit(ValuePrinter{}, instr.operand);
      std::cout << "\n";
      break;

    case OpCode::OP_ADD:
      std::cout << "OP_ADD\n";
      break;

    case OpCode::OP_SUB:
      std::cout << "OP_SUBTRACT\n";
      break;

    case OpCode::OP_MUL:
      std::cout << "OP_MULTIPLY\n";
      break;

    case OpCode::OP_DIV:
      std::cout << "OP_DIVIDE\n";
      break;

    case OpCode::OP_PRINT:
      std::cout << "OP_PRINT\n";
      break;

    case OpCode::OP_RETURN:
      std::cout << "OP_RETURN\n";
      break;

    case OpCode::OP_POP:
      std::cout << "OP_POP\n";
      break;

    default:
      std::cout << "UNKNOWN_OPCODE\n";
      break;
  }
}
