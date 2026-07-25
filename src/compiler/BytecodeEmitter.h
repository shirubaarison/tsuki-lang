#ifndef BYTECODE_EMITTER_H
#define BYTECODE_EMITTER_H

#include "ir/Chunk.h"
#include "ir/OpCode.h"
#include "runtime/value/Value.h"
#include <cstddef>

struct Local {
  std::string name;
  int depth;
};

class BytecodeEmitter {
public:
  explicit BytecodeEmitter(Chunk& chunk);

  size_t op(OpCode op);
  size_t op(OpCode op, const Value& value);
  size_t jump(OpCode op);

  void patchJump(size_t pos);
  void loop(size_t loopStart); 
private:
  Chunk& m_chunk;
};

#endif // !BYTECODE_EMITTER_H
