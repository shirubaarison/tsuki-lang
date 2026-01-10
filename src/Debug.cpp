#include "VM/Debug.h"
#include <iomanip>
#include <iostream>
#include <variant>

#include "VM/VM.h"
#include "Value.h"

void disassembleChunk(const std::vector<Instruction> &code, const char *name) {
  std::cout << name << ":\n";

  for (size_t offset = 0; offset < code.size(); ++offset) {
    disassembleInstruction(code[offset], offset);
  }
}

void disassembleInstruction(const Instruction &instr, size_t offset) {
  std::cout << std::right << std::setw(4) << std::setfill('0') << offset << " ";
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

    case OpCode::OP_GREATER:
      std::cout << "OP_GREATER\n";
      break;

    case OpCode::OP_GREATER_EQUAL:
      std::cout << "OP_GREATER_EQUAL\n";
      break;

    case OpCode::OP_LESS:
      std::cout << "OP_LESS\n";
      break;

    case OpCode::OP_LESS_EQUAL:
      std::cout << "OP_LESS_EQUAL\n";
      break;

    case OpCode::OP_NOT:
      std::cout << "OP_NOT\n";
      break;

    case OpCode::OP_EQUAL:
      std::cout << "OP_EQUAL\n";
      break;

    case OpCode::OP_NOT_EQUAL:
      std::cout << "OP_NOT_EQUAL\n";
      break;

    case OpCode::OP_TRUE:
      std::cout << "OP_TRUE\n";
      break;

    case OpCode::OP_FALSE:
      std::cout << "OP_FALSE\n";
      break;

    case OpCode::OP_NIL:
      std::cout << "OP_NIL\n";
      break;

    case OpCode::OP_AND:
      std::cout << "OP_AND\n";
      break;

    case OpCode::OP_OR:
      std::cout << "OP_OR\n";
      break;

    case OpCode::OP_DEFINE_GLOBAL:
      std::cout << "OP_DEFINE_GLOBAL\n";
      break;

    case OpCode::OP_GET_GLOBAL:
      std::cout << "OP_GET_GLOBAL\n";
      break;
  }
}
