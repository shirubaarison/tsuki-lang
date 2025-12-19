#ifndef DEBUG_H
#define DEBUG_H

#include "VM/VM.h"
void disassembleChunk(const std::vector<Instruction>& code, const char *name);
void disassembleInstruction(const Instruction& instr, size_t offset);

#endif // !DEBUG_H
