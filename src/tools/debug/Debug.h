#ifndef DEBUG_H
#define DEBUG_H

#include "ir/Chunk.h"

void disassembleChunk(const Chunk& chunk, const char* name);
size_t disassembleInstruction(const Chunk& chunk, size_t offset);

#endif // !DEBUG_H
