#include "VM/VM.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "VM/Debug.h"

VM::Machine::Machine(const std::vector<Instruction> &bytecode, bool debug)
    : debugMode(debug), code(bytecode) {
  stack.reserve(1024);
}

void VM::Machine::run() {
  while (ip < code.size()) {

    if (debugMode) {
      std::cout << "     ";
      for (const auto& val : stack) {
        std::cout << "[ ";
        std::visit(ValuePrinter{}, val);
        std::cout << " ]";
      }
      std::cout << "\n";

      disassembleInstruction(code[ip], ip);
    }

    const auto &instruction = code[ip++];

    switch (instruction.op) {
      case OpCode::OP_CONSTANT:
        stack.push_back(instruction.operand);
        break;

      case OpCode::OP_RETURN:
        return;

      case OpCode::OP_ADD: {
        if (std::holds_alternative<std::string>(stack.back())) {
          std::string b = std::get<std::string>(stack.back());
          stack.pop_back();
          if (std::holds_alternative<std::string>(stack.back())) {
            std::string a = std::get<std::string>(stack.back());
            stack.pop_back();

            stack.push_back(a + b);
          }
        } else {
          double b = std::get<double>(stack.back()); stack.pop_back();
          if (std::holds_alternative<std::string>(stack.back())) {
            std::string a = std::get<std::string>(stack.back());
            stack.pop_back();
            stack.push_back(a + std::to_string(b));
          } else {
            double a = std::get<double>(stack.back()); stack.pop_back();
            stack.push_back(a + b);
          }
        }
        break;
      }
      case OpCode::OP_SUB: {
        double b = std::get<double>(stack.back()); stack.pop_back();
        double a = std::get<double>(stack.back()); stack.pop_back();
        stack.push_back(a - b);
        break;
      }
      case OpCode::OP_MUL: {
        double b = std::get<double>(stack.back()); stack.pop_back();
        double a = std::get<double>(stack.back()); stack.pop_back();
        stack.push_back(a * b);
        break;
      }

      case OpCode::OP_DIV: {
        double b = std::get<double>(stack.back()); stack.pop_back();
        double a = std::get<double>(stack.back()); stack.pop_back();
        stack.push_back(a / b);
        break;
      }

      case OpCode::OP_POP:
        stack.pop_back();
        break;

      case OpCode::OP_PRINT:
        if (stack.empty()) {
          throw std::runtime_error("Stack underflow in OP_PRINT");
        }

        Value value = stack.back();
        stack.pop_back();

        std::visit(ValuePrinter{}, value);
        std::cout << "\n";
        break;
    }
  }
}
