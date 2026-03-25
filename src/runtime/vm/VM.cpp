#include "runtime/vm/VM.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "tools/debug/Debug.h"
#include "runtime/value/Value.h"

namespace {
bool isTruthy(Value value)
{
  return std::visit(
    overload{
      [](double) { return true; },
      [](int) { return true; },
      [](const std::string&) { return true; },
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
    case OpCode::ADD: result = lhs + rhs; break;
    case OpCode::SUB: result = lhs - rhs; break;
    case OpCode::MUL: result = lhs * rhs; break;
    case OpCode::DIV: result = lhs / rhs; break;
    default:
      throw std::runtime_error("Invalid numeric operation.");
  }

  if (isType<int>(a) && isType<int>(b) && op != OpCode::DIV) {
    stack.push_back(static_cast<int>(result));
  } else {
    stack.push_back(result);
  }
}
} // namespace

VM::Machine::Machine() { stack.reserve(1024); }

void VM::Machine::setDebugMode(bool setDebugMode) { debugMode = setDebugMode; }

void VM::Machine::setByteCode(const Chunk& bytecode)
{
  ip = 0;
  chunk = bytecode;
}

Byte VM::Machine::readByte() {
  return chunk.code[ip++];
}

Value VM::Machine::readConstant() {
  return chunk.constants[readByte()];
}

InterpretResult VM::Machine::run() {
  for (;;) {

    if (debugMode)
    {
      std::cout << "       stack ";
      if (stack.empty()) {
        std::cout << "[ ]";
      }
      for (const auto &val : stack) {
        std::cout << "[ ";
        std::visit(ValuePrinter{}, val);
        std::cout << " ]";
      }
      std::cout << std::endl;
      std::cout << "       globals ";
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

      disassembleInstruction(chunk, ip);
    }

    OpCode instruction = static_cast<OpCode>(readByte());

    switch (instruction)
    {
      case OpCode::LOAD_CONSTANT: {
        Value constant = readConstant();
        stack.push_back(constant);
        break;
      }

      case OpCode::ADD:
      case OpCode::SUB:
      case OpCode::MUL:
      case OpCode::DIV:
        calculate(stack, instruction);
        break;

      case OpCode::POP: {
        if (stack.empty()) {
          std::cout << "Stack is empty" << std::endl;
        } else {
          stack.pop_back();
        }
        break;
      }

      case OpCode::PRINT: {
        if (stack.empty()) {
          return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }

        Value value = stack.back();
        stack.pop_back();

        std::visit(ValuePrinter{}, value);
        std::cout << "\n";
        break;
      }

      case OpCode::NOT: {
        Value value = stack.back();
        stack.pop_back();

        stack.push_back(!isTruthy(value));
        break;
      }

      case OpCode::TRUE:
        stack.push_back(true);
        break;

      case OpCode::FALSE:
        stack.push_back(false);
        break;

      case OpCode::EQUAL:
        binaryCompare(stack, [](Value a, Value b){ return a == b; });
        break;

      case OpCode::NOT_EQUAL:
        binaryCompare(stack, [](Value a, Value b) { return a != b; });
        break;

      case OpCode::LESS:
        binaryCompare(stack, [](Value a, Value b){ return a < b; });
        break;

      case OpCode::GREATER:
        binaryCompare(stack, [](Value a, Value b){ return a > b; });
        break;

      case OpCode::DEFINE_GLOBAL: {
        auto name = std::get<std::string>(readConstant());
        globals[name] = stack.back();
        break;
      }

      case OpCode::GET_GLOBAL: {
        auto it = globals.find(std::get<std::string>(readConstant()));

        if (it != globals.end()) {
          stack.push_back(it->second);
          break;
        }

        std::cerr << "Variable not found." << std::endl;
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
      }

      case OpCode::SET_GLOBAL: {
        Value var { stack.back() };
        stack.pop_back();
        auto name = std::get<std::string>(readConstant());
        globals[name] = var;
        break;
      }

      case OpCode::DEFINE_LOCAL: {
        break;
      }

      case OpCode::GET_LOCAL: {
        int slot = std::get<int>(readConstant());
        stack.push_back(stack[slot]);
        break;
      }

      case OpCode::SET_LOCAL: {
        int slot = std::get<int>(readConstant());
        stack[slot] = stack.back();
        break;
      }

      case OpCode::JMP_IF_FALSE: {
        int offset = static_cast<int>(readByte());
        Value var { stack.back() };

        if (!isTruthy(var)) {
          ip += offset;
        }

        break;
      }

      case OpCode::JMP: {
        int offset = static_cast<int>(readByte());
        ip += offset;
        break;
      }

      case OpCode::LOOP: {
        int offset = static_cast<int>(readByte());
        ip -= offset;
        break;
      }

      case OpCode::RET:
        return InterpretResult::INTERPRET_OK;

      default:
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
    }
  }
}
