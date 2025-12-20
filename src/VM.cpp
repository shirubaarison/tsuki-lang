#include "VM/VM.h"

#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "VM/Debug.h"
#include "Value.h"

namespace {

bool isTruthy(Value value) {
  return std::visit(
    overload{
      [](double x) { return true; },
      [](int x) { return true; },
      [](const std::string &x) { return true; },
      [](bool x) { return x; },
      [](std::monostate) { return false; }},
    value);
}

template <typename T>
bool isType(Value value) {
  return std::visit([](auto&& value) {
    using v_type = std::decay_t<decltype(value)>;

    if constexpr (std::is_same_v<v_type, T>) {
      return true;
    } else {
      return false;
    }
  }, value);
 }

template <typename Op>
void binaryCompare(std::vector<Value>& stack, Op op) {
  Value b = stack.back();
  stack.pop_back();
  Value a = stack.back();
  stack.pop_back();

  stack.push_back(op(a, b));
}

void calculate(std::vector<Value>& stack, OpCode op) {
  Value b = stack.back();
  stack.pop_back();
  Value a = stack.back();
  stack.pop_back();

  if (isType<double>(a) && isType<double>(b)) {
    double a_val = std::get<double>(a);
    double b_val = std::get<double>(b);
    switch (op) {
      case OpCode::OP_ADD:
        return stack.push_back(a_val + b_val);
      case OpCode::OP_SUB:
        return stack.push_back(a_val - b_val);
      case OpCode::OP_MUL:
        return stack.push_back(a_val * b_val);
      case OpCode::OP_DIV:
        return stack.push_back(a_val / b_val);
      default:
        return;
    }
  } else if (isType<std::string>(a) && isType<std::string>(b)) {
    std::string a_val = std::get<std::string>(a);
    std::string b_val = std::get<std::string>(b);

    if (op == OpCode::OP_ADD) {
      return stack.push_back(a_val + b_val);
    }
  }
  throw std::runtime_error("You can only do operations in numbers or strings.");
}
} // namespace

VM::Machine::Machine(const std::vector<Instruction> &bytecode, bool debug)
: debugMode(debug), code(bytecode) {
  stack.reserve(1024);
}

void VM::Machine::run() {
  while (ip < code.size()) {

    if (debugMode) {
      std::cout << "     ";
      if (stack.empty()) {
        std::cout << "[ ]";
      }
      for (const auto &val : stack) {
        std::cout << "[ ";
        std::visit(ValuePrinter{}, val);
        std::cout << " ]";
      }
      std::cout << std::endl;

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
          double b = std::get<double>(stack.back());
          stack.pop_back();
          if (std::holds_alternative<std::string>(stack.back())) {
            std::string a = std::get<std::string>(stack.back());
            stack.pop_back();
            stack.push_back(a + std::to_string(b));
          } else {
            double a = std::get<double>(stack.back());
            stack.pop_back();
            stack.push_back(a + b);
          }
        }
        break;
      }

      case OpCode::OP_SUB:
      case OpCode::OP_MUL:
      case OpCode::OP_DIV:
        calculate(stack, instruction.op);
        break;

      case OpCode::OP_POP: {
        if (stack.empty()) {
          std::cout << "Stack is empty" << std::endl;
        } else {
          stack.pop_back();
        }
        break;
      }

      case OpCode::OP_PRINT: {
        if (stack.empty()) {
          throw std::runtime_error("Stack underflow in OP_PRINT");
        }

        Value value = stack.back();
        stack.pop_back();

        std::visit(ValuePrinter{}, value);
        std::cout << "\n";
        break;
      }

      case OpCode::OP_NOT: {
        Value value = stack.back();
        stack.pop_back();

        stack.push_back(!isTruthy(value));
      }

      case OpCode::OP_TRUE:
        stack.push_back(true);
        break;

      case OpCode::OP_FALSE:
        stack.push_back(false);
        break;

      case OpCode::OP_EQUAL:
        binaryCompare(stack, [](Value a, Value b){ return a == b; });
        break;

      case OpCode::OP_NOT_EQUAL:
        binaryCompare(stack, [](Value a, Value b) { return a != b; });
        break;

      case OpCode::OP_LESS:
        binaryCompare(stack, [](Value a, Value b){ return a < b; });
        break;

      case OpCode::OP_GREATER:
        binaryCompare(stack, [](Value a, Value b){ return a > b; });
        break;

      case OpCode::OP_AND: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (!isTruthy(a)) {
          stack.push_back(false);
        } else {
          stack.push_back(isTruthy(b));
        }

        break;
      }

      case OpCode::OP_OR: {
        Value b = stack.back();
        stack.pop_back();

        Value a = stack.back();
        stack.pop_back();

        if (isTruthy(a)) {
          stack.push_back(true);
        } else {
          stack.push_back(isTruthy(b));
        }

        break;
      }

      default:
        throw std::runtime_error("forgot to add maybe");
    }
  }
}
