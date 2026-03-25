#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "ir/OpCode.h"
#include "runtime/value/Value.h"

struct Instruction {
  OpCode op;
  Value operand;
};

#endif // !INSTRUCTION_H
