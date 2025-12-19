#ifndef VM_H
#define VM_H

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
  OP_POP
};

struct Instruction {
  OpCode op;
  Value operand;
};

namespace VM {
class Machine {
private:
  size_t ip = 0;
  bool debugMode = false;

  std::vector<Instruction> code;
  std::vector<Value> stack;

public:
  Machine(const std::vector<Instruction> &bytecode, bool debug = false);
  void run();
};

} // namespace VM

#endif // !VM_H
