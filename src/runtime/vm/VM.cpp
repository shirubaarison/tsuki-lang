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
  if (stack.size() < 2) {
    throw std::runtime_error("stack underflow.");
  }

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
  if (ip >= chunk.code.size()) {
    throw std::runtime_error("unexpected end of bytecode.");
  }

  return chunk.code[ip++];
}

InterpretResult VM::Machine::runtimeError(const std::string& message) {
  std::cerr << "Runtime error at byte " << ip << ": " << message << std::endl;
  return InterpretResult::INTERPRET_RUNTIME_ERROR;
}

Value VM::Machine::readConstant() {
  Byte index = readByte();
  if (index >= chunk.constants.size()) {
    throw std::runtime_error("constant index " + std::to_string(index) + " is out of range.");
  }
  return chunk.constants[index];
}

InterpretResult VM::Machine::run() {
  try {
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
          if (stack.size() < 2) {
            return runtimeError("stack underflow.");
          }
          calculate(stack, instruction);
          break;

        case OpCode::POP: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          } else {
            stack.pop_back();
          }
          break;
        }

        case OpCode::PRINT: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

          Value value = stack.back();
          stack.pop_back();

          std::visit(ValuePrinter{}, value);
          std::cout << "\n";
          break;
        }

        case OpCode::NOT: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

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

        case OpCode::LESS_EQUAL:
          binaryCompare(stack, [](Value a, Value b) { return a <= b; });
          break;

        case OpCode::GREATER:
          binaryCompare(stack, [](Value a, Value b){ return a > b; });
          break;

        case OpCode::GREATER_EQUAL:
          binaryCompare(stack, [](Value a, Value b) { return a >= b; });
          break;

        case OpCode::DEFINE_GLOBAL: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

          Value constant = readConstant();
          if (!std::holds_alternative<std::string>(constant)) {
            return runtimeError("DEFINE_GLOBAL requires a string constant.");
          }

          auto name = std::get<std::string>(constant);
          globals[name] = stack.back();
          break;
        }

        case OpCode::GET_GLOBAL: {
          Value constant = readConstant();

          if (!std::holds_alternative<std::string>(constant)) {
            return runtimeError("GET_GLOBAL requires a string constant.");
          }

          const auto& name = std::get<std::string>(constant);
          auto it = globals.find(name);

          if (it == globals.end()) {
            return runtimeError("variable not found.");
          }

          stack.push_back(it->second);
          break;
        }

        case OpCode::SET_GLOBAL: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

          const Value& value { stack.back() };
          Value constant = readConstant();
          if (!std::holds_alternative<std::string>(constant)) {
            return runtimeError("SET_GLOBAL requires a string constant.");
          }

          auto name = std::get<std::string>(constant);
          globals[name] = value;
          break;
        }

        case OpCode::DUP: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

          stack.push_back(stack.back());
          break;
        }

        case OpCode::GET_LOCAL: {
          Value constant = readConstant();
          if (!std::holds_alternative<int>(constant)) {
            return runtimeError("GET_LOCAL requires a integer constant.");
          }

          int slot = std::get<int>(constant);
          if (slot < 0 || static_cast<size_t>(slot) >= stack.size()) {
            return runtimeError("Local slot out of range.");
          }

          stack.push_back(stack[slot]);
          break;
        }

        case OpCode::SET_LOCAL: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }
          Value constant = readConstant();
          if (!std::holds_alternative<int>(constant)) {
            return runtimeError("SET_LOCAL requires a integer constant.");
          }

          int slot = std::get<int>(constant);
          if (slot < 0 || static_cast<size_t>(slot) >= stack.size()) {
            return runtimeError("Local slot out of range.");
          }

          stack[slot] = stack.back();
          break;
        }

        case OpCode::JMP_IF_FALSE: {
          const std::size_t offset = readByte();

          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

          if (!isTruthy(stack.back())) {
            if (offset > chunk.code.size() - ip) {
              return runtimeError("invalid jump offset.");
            }
            ip += offset;
          }

          break;
        }

        case OpCode::JMP: {
          const std::size_t offset = readByte();
          if (offset > chunk.code.size() - ip) {
            return runtimeError("invalid jump offset.");
          }

          ip += offset;
          break;
        }

        case OpCode::LOOP: {
          const std::size_t offset = readByte();
          if (offset > ip) {
            return runtimeError("invalid loop offset.");
          }

          ip -= offset;
          break;
        }

        case OpCode::NEGATE: {
          if (stack.empty()) {
            return runtimeError("stack is empty.");
          }

          Value& value = stack.back();
          if (!isType<int>(value)) {
            return runtimeError("value is not a number.");
          }

          value = -(std::get<int>(value));
          break;
        }

        case OpCode::RET:
          return InterpretResult::INTERPRET_OK;

        default:
          return runtimeError("unknown opcode.");
      }
    }
  } catch (const std::runtime_error& e) {
    return runtimeError(e.what());
  }
}
