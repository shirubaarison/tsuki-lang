
#include "Lexer.h"
#include <cstring>
#include <iostream>

#include <readline/history.h>
#include <readline/readline.h>

#define DEBUG_TOKENS

static void run(char *line) {
  std::string str(line);
  std::cout << str << std::endl;

  Lexer lexer = Lexer(str);

  std::vector<Token> tokens = lexer.scanTokens();

#ifdef DEBUG_TOKENS
  for (Token t : tokens) {
    std::cout << t.lexeme << " (" << tokenTypeToString(t.type) << ")" << std::endl;
  }
#endif
}

static void repl() {
  char *line;

  using_history();

  for (;;) {
    line = readline("> ");

    if (!line) break;

    add_history(line);

    if (strcmp(line, "quit") == 0) {
      free(line);
      break;
    }

    run(line);
  }
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    repl();
  } else {
    std::cerr << "Usage: " << argv[0] << " [path]" << std::endl;
  }

  return 0;
}
