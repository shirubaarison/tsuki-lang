#ifndef VM_H
#define VM_H

#include <map>
#include <vector>
#include "runtime/value/Value.h"
#include "ir/Instruction.h"

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
