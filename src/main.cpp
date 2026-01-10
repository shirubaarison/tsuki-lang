#include <fstream>
#include <iostream>

#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <sstream>
#include <string>
#include <vector>

#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM/Debug.h"
#include "VM/VM.h"

#define VERSION "0.0.1"

bool isInDebugMode = false;

VM::Machine vm{};

namespace {
void run(std::string& source)
{
  Lexer lexer(source);

  std::vector<Token> tokens = lexer.scanTokens();

  if (isInDebugMode)
  {
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

  if (isInDebugMode)
  {
    for (const auto& stmt : syntaxTree)
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

  vm.setByteCode(bytecode);
  vm.run();
}

void help(bool man = false)
{
  std::cout << "Tsuki is a dynamically-typed interpreted language that "
    "compiles source code to bytecode instructions executed by a "
    "stack-based virtual machine" << std::endl;
  if (!man)
  {
    std::cout << "Available commands:\n" << "  - help\n  - quit" << std::endl;
  }
  else
  {
    std::cout << "Available options:\n";
    std::cout << "  -h, --help  \t give this help list" << std::endl;
    std::cout << "  -d, --debug \t print debug information about lexer, parser, compilated code and vm backtrace" << std::endl;
  }
}

void repl()
{
  std::cout << "Tsuki version " << VERSION
            << "\nType \"help\" for more information.\n";

  using_history();

  while (true)
  {
    char* line = readline(">> ");
    if (!line) {
      std::cout << std::endl;
      break;
    }

    std::string input(line);
    free(line);

    if (input.empty()) {
      continue;
    }

    add_history(input.c_str());

    if (input == "quit") {
      break;
    }
    else if (input == "clear") {
      std::cout << "\033[2J\033[H";
      continue;
    }
    else if (input == "help") {
      help();
      continue;
    }

    run(input);
  }
}

struct Options {
  bool debug = false;
  bool help = false;
  std::string file;
};

Options parseArgs(int argc, char* argv[]) {
  Options opts;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      opts.help = true;
    }
    else if (arg == "-d" || arg == "--debug") {
      opts.debug = true;
    }
    else if (arg.starts_with("-")) {
      std::cerr << "Unknown option: " << arg << std::endl;
      exit(64);
    }
    else {
      if (!opts.file.empty()) {
        std::cerr << "Multiple input files provided\n";
        exit(64);
      }
      opts.file = arg;
    }
  }

  return opts;
}

std::string openFile(const std::string& path)
{
  std::ifstream file(path);
  if (!file) {
    std::cerr << "Unable to open file: " << path << std::endl;
    exit(66);
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
}

int main(int argc, char* argv[])
{
  Options opts = parseArgs(argc, argv);

  if (opts.help) {
    help(true);
    return 0;
  }

  isInDebugMode = opts.debug;
  vm.setDebugMode(isInDebugMode);

  if (!opts.file.empty()) {
    std::string content = openFile(opts.file);
    run(content);
  }
  else {
    repl();
  }

  return 0;
}
