#include "ir/Chunk.h"

void Chunk::write(Byte byte, int line)
{
  code.push_back(byte);
  lines.push_back(line);
}

void Chunk::writeOp(OpCode op, int line)
{
  write(static_cast<Byte>(op), line);
}

int Chunk::addConstant(const Value& value)
{
  constants.push_back(value);
  return static_cast<int>(constants.size() - 1);
}
