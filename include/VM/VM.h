#ifndef VM_H
#define VM_H

#include <map>
#include <vector>
#include "Value.h"

enum OpCode {
  OP_CONSTANT,
  OP_RETURN,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_PRINT,
  OP_POP,
  OP_EQUAL,
  OP_NOT_EQUAL,
  OP_NOT,
  OP_GREATER,
  OP_GREATER_EQUAL,
  OP_LESS,
  OP_LESS_EQUAL,
  OP_FALSE,
  OP_TRUE,
  OP_NIL,
  OP_AND,
  OP_OR,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL
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
  Machine(const std::vector<Instruction>& bytecode, bool debug = false);
  Machine();
  InterpretResult run();

  void setDebugMode(bool setDebugMode);
  void setByteCode(const std::vector<Instruction>& bytecode);
};

} // namespace VM

#endif // !VM_H
