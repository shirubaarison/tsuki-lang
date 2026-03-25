#ifndef CHUNK_H
#define CHUNK_H

#include "ir/OpCode.h"
#include "runtime/value/Value.h"
#include <cstdint>
#include <vector>

using Byte = uint8_t;

struct Chunk {
  std::vector<Byte> code;
  std::vector<Value> constants;
  std::vector<int> lines;

  void write(Byte byte, int line);

  void writeOp(OpCode op, int line);

  int addConstant(const Value& value);
};

#endif // !CHUNK_H
