# Tsuki Lang

A small stack-based programming language implemented in C++ with a bytecode compiler and virtual machine.

## Overview

Tsuki is a dynamically-typed interpreted language that compiles source code to bytecode instructions executed by a stack-based virtual machine. The implementation follows a traditional compiler architecture with lexical analysis, parsing, compilation, and execution phases.

## Example Code

```javascript
var x = 10;
var y = 20;

if (x < y) {
    print "x is less than y";
} else {
    print "x is greater or equal to y";
}

var result = x + y * 2;
print result;
```

## Building and Running

```bash
# Compile the project
make

# Run a tsuki program
./tsuki program.tsuki

# or REPL
./tsuki
```
