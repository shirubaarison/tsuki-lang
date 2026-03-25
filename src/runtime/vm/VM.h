#ifndef VM_H
#define VM_H

#include <map>
#include <vector>
#include "runtime/value/Value.h"
#include "ir/Chunk.h"

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

  Chunk chunk;
  std::vector<Value> stack;
  std::map<std::string, Value> globals;

  Byte readByte();
  Value readConstant();

public:
  Machine();
  InterpretResult run();

  void setDebugMode(bool setDebugMode);
  void setByteCode(const Chunk& chunk);
};

} // namespace VM

#endif // !VM_H
