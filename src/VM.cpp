#include "VM/VM.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "VM/Debug.h"
#include "Value.h"

namespace {
bool isTruthy(Value value)
{
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
bool isType(const Value& value) { return std::holds_alternative<T>(value); }

template <typename Op>
void binaryCompare(std::vector<Value>& stack, Op op) 
{
  Value b = stack.back();
  stack.pop_back();
  Value a = stack.back();
  stack.pop_back();

  stack.push_back(op(a, b));
}

void calculate(std::vector<Value>& stack, OpCode op)
{
  Value b = stack.back();
  stack.pop_back();
  Value a = stack.back();
  stack.pop_back();

  auto to_double = [](const Value& v) -> double {
    if (isType<int>(v))    return static_cast<double>(std::get<int>(v));
    if (isType<double>(v)) return std::get<double>(v);
    throw std::runtime_error("Non-numeric value.");
  };

  double lhs = to_double(a);
  double rhs = to_double(b);

  double result;
  switch (op) {
    case OpCode::OP_ADD: result = lhs + rhs; break;
    case OpCode::OP_SUB: result = lhs - rhs; break;
    case OpCode::OP_MUL: result = lhs * rhs; break;
    case OpCode::OP_DIV: result = lhs / rhs; break;
    default:
      throw std::runtime_error("Invalid numeric operation.");
  }

  if (isType<int>(a) && isType<int>(b) && op != OpCode::OP_DIV) {
    stack.push_back(static_cast<int>(result));
  } else {
    stack.push_back(result);
  }
}
} // namespace

VM::Machine::Machine(const std::vector<Instruction>& bytecode, bool debug)
: debugMode(debug), code(bytecode) { stack.reserve(1024); }

VM::Machine::Machine() { stack.reserve(1024); }

void VM::Machine::setDebugMode(bool setDebugMode) { debugMode = setDebugMode; }

void VM::Machine::setByteCode(const std::vector<Instruction>& bytecode)
{
  ip = 0;
  code = bytecode;
}

InterpretResult VM::Machine::run() {
  for (;;) {

    if (debugMode)
    {
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
      std::cout << "     ";
      if (globals.empty()) {
        std::cout << "[ ]";
      }
      for (auto it = globals.cbegin(); it != globals.cend(); ++it) {
        std::cout << "[ ";
        std::cout << it->first << " : ";
        std::visit(ValuePrinter{}, it->second);
        std::cout << " ]";
      }
      std::cout << std::endl;

      disassembleInstruction(code[ip], ip);
    }

    const auto& instruction = code[ip++];

    switch (instruction.op)
    {
      case OpCode::OP_CONSTANT:
        stack.push_back(instruction.operand);
        break;

      case OpCode::OP_ADD:
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
          return InterpretResult::INTERPRET_RUNTIME_ERROR;
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
        break;
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

      case OpCode::OP_DEFINE_GLOBAL: {
        Value var { stack.back() };
        stack.pop_back();
        auto& name = std::get<std::string>(instruction.operand);
        globals.insert({ name, var});
        break;
      }

      case OpCode::OP_GET_GLOBAL: {
        auto it = globals.find(std::get<std::string>(instruction.operand));

        if (it != globals.end()) {
          stack.push_back(it->second);
          break;
        }

        std::cerr << "Variable not found." << std::endl;
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
      }

      case OpCode::OP_RETURN:
        return InterpretResult::INTERPRET_OK;

      default:
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
    }
  }
}
