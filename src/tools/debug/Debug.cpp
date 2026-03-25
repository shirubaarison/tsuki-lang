#include "tools/debug/Debug.h"
#include <iomanip>
#include <iostream>
#include <variant>

#include "runtime/value/Value.h"

void disassembleChunk(const std::vector<Instruction>& code, const char* name)
{
  std::cout << name << ":\n";

  for (size_t offset = 0; offset < code.size(); ++offset) {
    disassembleInstruction(code[offset], offset);
  }
}

void disassembleInstruction(const Instruction& instr, size_t offset)
{
  std::cout << std::right << std::setw(4) << std::setfill('0') << offset << "   | ";
  switch (instr.op)
  {
    case OpCode::LOAD_CONSTANT:
      std::cout << std::setw(16) << std::setfill(' ') << std::left
        << "LOAD_CONSTANT";
      std::visit(ValuePrinter{}, instr.operand);
      std::cout << "\n";
      break;

    case OpCode::ADD:
      std::cout << "ADD\n";
      break;

    case OpCode::SUB:
      std::cout << "OP_SUBTRACT\n";
      break;

    case OpCode::MUL:
      std::cout << "OP_MULTIPLY\n";
      break;

    case OpCode::DIV:
      std::cout << "OP_DIVIDE\n";
      break;

    case OpCode::PRINT:
      std::cout << "PRINT\n";
      break;

    case OpCode::RET:
      std::cout << "RET\n";
      break;

    case OpCode::POP:
      std::cout << "POP\n";
      break;

    case OpCode::GREATER:
      std::cout << "GREATER\n";
      break;

    case OpCode::GREATER_EQUAL:
      std::cout << "GREATER_EQUAL\n";
      break;

    case OpCode::LESS:
      std::cout << "LESS\n";
      break;

    case OpCode::LESS_EQUAL:
      std::cout << "LESS_EQUAL\n";
      break;

    case OpCode::NOT:
      std::cout << "NOT\n";
      break;

    case OpCode::EQUAL:
      std::cout << "EQUAL\n";
      break;

    case OpCode::NOT_EQUAL:
      std::cout << "NOT_EQUAL\n";
      break;

    case OpCode::TRUE:
      std::cout << "TRUE\n";
      break;

    case OpCode::FALSE:
      std::cout << "FALSE\n";
      break;

    case OpCode::NIL:
      std::cout << "NIL\n";
      break;

    case OpCode::DEFINE_GLOBAL:
      std::cout << "DEFINE_GLOBAL  " << std::get<std::string>(instr.operand) << "\n";
      break;

    case OpCode::GET_GLOBAL:
      std::cout << "GET_GLOBAL  " << std::get<std::string>(instr.operand) << "\n";
      break;

    case OpCode::SET_GLOBAL:
      std::cout << "SET_GLOBAL  " << std::get<std::string>(instr.operand) << "\n";
      break;

    case OpCode::DEFINE_LOCAL:
      std::cout << "DEFINE_LOCAL  " << std::get<std::string>(instr.operand) << "\n";
      break;

    case OpCode::GET_LOCAL:
      std::cout << "GET_LOCAL  " << std::get<std::string>(instr.operand) << "\n";
      break;

    case OpCode::SET_LOCAL:
      std::cout << "SET_LOCAL\n";
      break;

    case OpCode::JMP_IF_FALSE:
      std::cout << "JMP_IF_FALSE " << offset << " -> " << offset + std::get<int>(instr.operand) << "\n";
      break;

    case OpCode::JMP:
      std::cout << "JMP " << offset << " -> " << offset + std::get<int>(instr.operand) << "\n";
      break;

    case OpCode::LOOP:
      std::cout << "LOOP\n";
      break;
  }
}
