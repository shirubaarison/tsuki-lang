#include <algorithm>
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

bool isInDebugMode = false;

static void run(char *line) {
  std::string str(line);

  Lexer lexer(str);

  std::vector<Token> tokens = lexer.scanTokens();

  if (isInDebugMode) {
    for (Token t : tokens) {
      if (t.type != TokenType::TOKEN_EOF) {
        std::cout << t.lexeme << "\t" << tokenTypeToString(t.type) << std::endl;
      }
    }
    std::cout << std::endl;
  }

  Parser parser(tokens);

  std::vector<std::unique_ptr<Stmt>> syntaxTree = parser.parse();

  if (parser.getHadError()) {
    return;
  }

  if (isInDebugMode) {
    for (const auto &stmt : syntaxTree)
    if (stmt) {
      stmt->print(std::cout);
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  std::vector<Instruction> bytecode;
  Compiler compiler(bytecode, std::move(syntaxTree));
  compiler.compile();

  if (isInDebugMode) {
    disassembleChunk(bytecode, "_main");
    std::cout << std::endl;
  }

  VM::Machine vm(bytecode, isInDebugMode);
  vm.run();
}

static void help(bool man = false) {
  std::cout << "Tsuki is a dynamically-typed interpreted language that "
               "compiles source code to bytecode instructions executed by a "
               "stack-based virtual machine" << std::endl;
  if (!man) {
    std::cout << "Available commands:\n" << "  - help\n  - quit" << std::endl;
  } else {
    std::cout << "Available options:\n";
    std::cout << "  -h, --help  \t give this help list" << std::endl;
    std::cout << "  -d, --debug \t print debug information about lexer, parser, compilated code and vm backtrace" << std::endl;
  }
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

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[]) {
  if (cmdOptionExists(argv, argv + argc, "--debug") || cmdOptionExists(argv, argv + argc, "-d")) {
    isInDebugMode = true;
    repl();
  } else if (cmdOptionExists(argv, argv + argc, "--help") || cmdOptionExists(argv, argv + argc, "-h")) {
    help(true);
  } else {
    if (argc == 1) {
      repl();
    } else {
      std::cout << "Usage: " << argv[0] << " [-options] [file_path]" << std::endl;
      help(true);
      exit(64);
    }
  }

  return 0;
}
