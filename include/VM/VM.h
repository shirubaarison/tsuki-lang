#ifndef VM_H
#define VM_H

#include <map>
#include <vector>
#include "Value.h"

enum OpCode {
  LOAD_CONSTANT,
  RET,
  ADD,
  SUB,
  MUL,
  DIV,
  PRINT,
  POP,
  EQUAL,
  NOT_EQUAL,
  NOT,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,
  FALSE,
  TRUE,
  NIL,
  DEFINE_GLOBAL,
  GET_GLOBAL,
  SET_GLOBAL,
  DEFINE_LOCAL,
  GET_LOCAL,
  SET_LOCAL,
  JMP_IF_FALSE,
  JMP,
  LOOP,
};

struct Instruction {
  OpCode op;
  Value operand;
};

enum class InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

namespace VM {
class Machine {
private:
  size_t ip = 0;
  bool debugMode = false;

  std::vector<Instruction> code;
  std::vector<Value> stack;
  std::map<std::string, Value> globals;
public:
  Machine();
  InterpretResult run();

  void setDebugMode(bool setDebugMode);
  void setByteCode(const std::vector<Instruction>& bytecode);
};

} // namespace VM

#endif // !VM_H
