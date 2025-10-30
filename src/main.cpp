
#include "Lexer.h"
#include "Parser.h"
#include "TokenType.h"
#include <cstring>
#include <iostream>

#include <memory>
#include <readline/history.h>
#include <readline/readline.h>

#define DEBUG_TOKENS
#define DEBUG_PARSER

static void run(char *line) {
  std::string str(line);
  std::cout << str << std::endl;

  Lexer lexer(str);

  std::vector<Token> tokens = lexer.scanTokens();

#ifdef DEBUG_TOKENS
  for (Token t : tokens) {
    if (t.type != TokenType::TOKEN_EOF) {
      std::cout << t.lexeme << " " << tokenTypeToString(t.type) << std::endl;
    }
  }
#endif

  Parser parser(tokens);

  std::vector<std::unique_ptr<Expr>> expressions = parser.parse();

#ifdef DEBUG_PARSER
  for (const auto &expr : expressions)
    if (expr) {
      expr->print(std::cout);
    }
  std::cout << std::endl;
#endif
}

static void repl() {
  char *line;

  using_history();

  for (;;) {
    line = readline("> ");

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
