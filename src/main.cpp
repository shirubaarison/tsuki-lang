#include <cstring>
#include <iostream>

#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <vector>

#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "TokenType.h"
#include "VM/Debug.h"
#include "VM/VM.h"

#define VERSION "0.0.1"

#define DEBUG_TOKENS
#define DEBUG_PARSER
#define DEBUG_COMPILER

static void run(char *line) {
  std::string str(line);

  Lexer lexer(str);

  std::vector<Token> tokens = lexer.scanTokens();

#ifdef DEBUG_TOKENS
  for (Token t : tokens) {
    if (t.type != TokenType::TOKEN_EOF) {
      std::cout << t.lexeme << "\t" << tokenTypeToString(t.type) << std::endl;
    }
  }
#endif

  Parser parser(tokens);

  std::vector<std::unique_ptr<Stmt>> syntaxTree = parser.parse();

#ifdef DEBUG_PARSER
  for (const auto &stmt : syntaxTree)
    if (stmt) {
      stmt->print(std::cout);
      std::cout << std::endl;
    }
#endif

  std::vector<Instruction> bytecode;
  Compiler compiler(bytecode, std::move(syntaxTree));
  compiler.compile();

#ifdef DEBUG_COMPILER
  disassembleChunk(bytecode, "_main");
#endif

  VM::Machine vm(bytecode
#ifdef DEBUG_COMPILER
                 ,
                 true
#endif
  );
  vm.run();
}

static void help() {
  std::cout << "Tsuki is a dynamically-typed interpreted language that "
               "compiles source code to bytecode instructions executed by a "
               "stack-based virtual machine" << std::endl;
  std::cout << "Available commands:\n" << "  - help\n  - quit" << std::endl;
}

static void repl() {
  std::cout << "Tsuki version " << VERSION << "\nType \"help\" for more information.\n";

  char *line;

  using_history();

  for (;;) {
    line = readline(">> ");

    if (!line) {
      break;
    }

    add_history(line);

    if (strcmp(line, "quit") == 0) {
      free(line);
      break;
    } else if (strcmp(line, "clear") == 0) {
      std::cout << "\033[2J";
      continue;
    } else if (strcmp(line, "help") == 0) {
      help();
      continue;
    }

    run(line);
  }
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    repl();
  } else {
    std::cerr << "Usage: " << argv[0] << " [path]" << std::endl;
    exit(64);
  }

  return 0;
}
