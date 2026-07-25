#include "compiler/BytecodeEmitter.h"

BytecodeEmitter::BytecodeEmitter(Chunk& chunk) : m_chunk(chunk) {}

size_t BytecodeEmitter::op(OpCode op)
{
  size_t pos = m_chunk.code.size();
  m_chunk.writeOp(op, 0);
  return pos;
}

size_t BytecodeEmitter::op(OpCode op, const Value& value)
{
  size_t pos = m_chunk.code.size();
  m_chunk.writeOp(op, 0);
  int idx = m_chunk.addConstant(value);
  m_chunk.write(static_cast<Byte>(idx), 0);

  return pos;
}

size_t BytecodeEmitter::jump(OpCode op)
{
  size_t pos = m_chunk.code.size();
  m_chunk.writeOp(op, 0);
  m_chunk.write(0xFF, 0); // placeholder

  return pos;
}

void BytecodeEmitter::patchJump(size_t pos)
{
  int offset = static_cast<int>(m_chunk.code.size()) - pos - 2;
  m_chunk.code[pos + 1] = static_cast<Byte>(offset);
}

void BytecodeEmitter::loop(size_t loopStart)
{

}
